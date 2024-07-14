#include "LagCompensationComponent.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Weapon/Weapon.h"


ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}


void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();

}


void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!m_pCharacter || !m_pCharacter->HasAuthority()) return;
	
	UpdateFrameHistory();
}

void ULagCompensationComponent::UpdateFrameHistory()
{
	FFramePackage currentFrame{};
    SaveFramePackage(currentFrame);
	//For the first and second elements ever added we can just add them
	if (m_FrameHistory.Num() <= 1)
	{
		m_FrameHistory.AddHead(currentFrame);
	}
	else
	{
		float historyLength = m_FrameHistory.GetHead()->GetValue().Time - m_FrameHistory.GetTail()->GetValue().Time;
		while(historyLength > m_MaxRecordingTime)
		{
			m_FrameHistory.RemoveNode(m_FrameHistory.GetTail());
			historyLength = m_FrameHistory.GetHead()->GetValue().Time - m_FrameHistory.GetTail()->GetValue().Time;
		}
		m_FrameHistory.AddHead(currentFrame);
	}
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& framePackage, FColor color)
{
	for (const auto& hitBox : framePackage.HitBoxInfo)
	{
		DrawDebugBox(GetWorld(),
			hitBox.Value.Location,
			hitBox.Value.BoxExtend,
			hitBox.Value.Rotation.Quaternion(),
			color,
			true);
	}
}

FServerSideRewindResult ULagCompensationComponent::ServerSideRewind(ABlasterCharacter* pHitCharacter,
	const FVector_NetQuantize& traceStart, const FVector_NetQuantize& traceEnd, const float hitTime)
{
	//This is the history of the character that GOT shot.
	//If we are calling this function we ARE the character that did the shooting
	const TDoubleLinkedList<FFramePackage>& frameHistory = pHitCharacter->GetLagCompensationComponent()->m_FrameHistory;

	const float oldestHitTime = frameHistory.GetTail()->GetValue().Time;
	const float newestHitTime = frameHistory.GetHead()->GetValue().Time;
	
	if (!pHitCharacter ||
		!pHitCharacter->GetLagCompensationComponent() ||
			frameHistory.Num() <= 1 ||
			oldestHitTime > hitTime // too far back - too laggy to perform a fair server side rewind
			) return {false, false};
	
	bool shouldInterpolate = true;
	//These if checks are there to verify that the hit we are rewinding is within the frame history
	FFramePackage frameToCheck{};
	if (oldestHitTime == hitTime) //Very unlikely but we have to check it
	{
		frameToCheck = frameHistory.GetTail()->GetValue();
		shouldInterpolate = false;
	}
	if (newestHitTime <= hitTime)
	{
		frameToCheck = frameHistory.GetHead()->GetValue();
		shouldInterpolate = false;
	}

	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* pYounger = frameHistory.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* pOlder = pYounger;
	while(pOlder->GetValue().Time > hitTime) // run the loop while OLDER is still younger than hit time
	{
		if (!pOlder->GetNextNode()) break;
		//Keep moving them back until older and younger are on either side of the hit time
		//What you are looking for is this: pOlder->Time < hitTime < pYounger->Time
		pOlder = pOlder->GetNextNode();
		if (pOlder->GetValue().Time > hitTime)
			pYounger = pOlder;
	}
	if (pOlder->GetValue().Time == hitTime)
	{
		frameToCheck = pOlder->GetValue();
		shouldInterpolate = false;
	}
	
	if (shouldInterpolate)
	{
		frameToCheck = InterpolateBetweenFrames(pOlder->GetValue(), pYounger->GetValue(), hitTime);
	}

	return ConfirmHit(frameToCheck, pHitCharacter, traceStart, traceEnd);
}

void ULagCompensationComponent::ServerDamageRequest_Implementation(ABlasterCharacter* pHitCharacter,
	const FVector_NetQuantize& traceStart, const FVector_NetQuantize& traceEnd, const float hitTime,
	AWeapon* pDamageCauser)
{
	const FServerSideRewindResult result = ServerSideRewind(pHitCharacter, traceStart, traceEnd, hitTime);
	
	if (!pHitCharacter || !result.WasHitConfirmed || !pDamageCauser) return;

	UGameplayStatics::ApplyDamage(pHitCharacter, pDamageCauser->GetDamage(), pHitCharacter->GetController(),
		pDamageCauser, UDamageType::StaticClass());
}

FServerSideRewindResult ULagCompensationComponent::ConfirmHit(const FFramePackage& framePackage,
                                                              ABlasterCharacter* pHitCharacter, const FVector_NetQuantize& traceStart, const FVector_NetQuantize& traceEnd)
{
	if (!pHitCharacter) return FServerSideRewindResult{};
	
	FFramePackage currentFrame{};
	GetPlayerHitBoxes(pHitCharacter, currentFrame);
	MovePlayerHitBoxes(pHitCharacter, framePackage);
	
	UBoxComponent* pHeadBox = pHitCharacter->HitBoxes["head"];
	pHeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pHeadBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	FHitResult hitResult{};
	const FVector end = traceStart + (traceEnd - traceStart) * 1.25f;

	//Temporarily disable the player's mesh collision so that it doesn't get in the way of the line trace
	pHitCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, end, ECC_Visibility);
	if (hitResult.bBlockingHit) //we got a headshot
	{
		ResetPlayerHitBoxes(pHitCharacter, currentFrame);
		pHitCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		return FServerSideRewindResult{true, true};
	}
	
	//If we reach here that means it's not a head shot
	for (const auto& hitBox : pHitCharacter->HitBoxes)
	{
		if (hitBox.Value == nullptr) continue;
		
		hitBox.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		hitBox.Value->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
	hitResult = {};
	GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, end, ECC_Visibility);
	if (hitResult.bBlockingHit)
	{
		ResetPlayerHitBoxes(pHitCharacter, currentFrame);
		pHitCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		return FServerSideRewindResult{true, false};
	}

	//If we reach here that means that it was a miss
	ResetPlayerHitBoxes(pHitCharacter, currentFrame);
	pHitCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	return FServerSideRewindResult{false, false};
}

void ULagCompensationComponent::GetPlayerHitBoxes(ABlasterCharacter* pHitCharacter, FFramePackage& outFramePackage)
{
	if (!pHitCharacter) return;

	for (const auto& hitBox : pHitCharacter->HitBoxes)
	{
		FBoxInformation boxInfo{};
		boxInfo.Location = hitBox.Value->GetComponentLocation();
		boxInfo.Rotation = hitBox.Value->GetComponentRotation();
		boxInfo.BoxExtend = hitBox.Value->GetScaledBoxExtent();

		outFramePackage.HitBoxInfo.Add(hitBox.Key, boxInfo);
	}
}
	
void ULagCompensationComponent::MovePlayerHitBoxes(ABlasterCharacter* pHitCharacter, const FFramePackage& outFramePackage)
{
	if (pHitCharacter == nullptr) return;
	for (auto& HitBoxPair : pHitCharacter->HitBoxes)
	{
		if (HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(outFramePackage.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(outFramePackage.HitBoxInfo[HitBoxPair.Key].Rotation);
			//HitBoxPair.Value->SetBoxExtent(outFramePackage.HitBoxInfo[HitBoxPair.Key].BoxExtend);
			//setting the box extent is causing it to shrink
		}
	}
}

void ULagCompensationComponent::ResetPlayerHitBoxes(ABlasterCharacter* pHitCharacter,
	const FFramePackage& package)
{
	if (!pHitCharacter) return;

	for (auto& hitBox : pHitCharacter->HitBoxes)
	{
		if (hitBox.Value == nullptr) continue;
		
		hitBox.Value->SetWorldLocation(package.HitBoxInfo[hitBox.Key].Location);
		hitBox.Value->SetWorldRotation(package.HitBoxInfo[hitBox.Key].Rotation);
		//hitBox.Value->SetBoxExtent(package.HitBoxInfo[hitBox.Key].BoxExtend);
		//setting the box extent is causing it to shrink
		hitBox.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& framePackage)
{
	if(!m_pCharacter) return;

	//This will get the server's time, but we are syncing the time in the blaster character
	framePackage.Time = GetWorld()->GetTimeSeconds();
	for(const auto& hitBox : m_pCharacter->HitBoxes)
	{
		FBoxInformation boxInfo{};
		
		boxInfo.Location = hitBox.Value->GetComponentLocation();
		boxInfo.Rotation = hitBox.Value->GetComponentRotation();
		boxInfo.BoxExtend = hitBox.Value->GetScaledBoxExtent();

		framePackage.HitBoxInfo.Add(hitBox.Key, boxInfo);
	}
}

FFramePackage ULagCompensationComponent::InterpolateBetweenFrames(const FFramePackage& olderFrame,
	const FFramePackage& youngerFrame, const float hitTime)
{
	const float distance = youngerFrame.Time - olderFrame.Time;
	const float alpha = FMath::Clamp((hitTime - olderFrame.Time) / distance, 0.f, 1.f);

	FFramePackage interpolatedFrame{};
	interpolatedFrame.Time = hitTime;

	for (const auto& youngerPair : youngerFrame.HitBoxInfo)
	{
		const FName& boxInfoName = youngerPair.Key;
		const FBoxInformation& olderBoxInfo = olderFrame.HitBoxInfo[boxInfoName];
		const FBoxInformation& youngerBoxInfo = youngerPair.Value;

		FBoxInformation interpolationBoxInfo{};
		interpolationBoxInfo.Location = FMath::VInterpTo(olderBoxInfo.Location, youngerBoxInfo.Location, 1, alpha);
		interpolationBoxInfo.Rotation = FMath::RInterpTo(olderBoxInfo.Rotation, youngerBoxInfo.Rotation, 1, alpha);
		interpolationBoxInfo.BoxExtend = youngerBoxInfo.BoxExtend;

		interpolatedFrame.HitBoxInfo.Add(boxInfoName, interpolationBoxInfo);
	}

	return interpolatedFrame;
}