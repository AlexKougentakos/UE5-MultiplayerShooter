#include "LagCompensationComponent.h"

#include "Components/BoxComponent.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"


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

	UpdateFrameHistory();
}

void ULagCompensationComponent::UpdateFrameHistory()
{
	FFramePackage currentFrame{};
	SaveFramePackage(currentFrame);
	
	//For the first and second elements ever added we can just add them
	if (m_FrameHistory.Num() <= 1)
	{
		m_FrameHistory.AddFront(currentFrame);
	}
	else
	{
		m_FrameHistory.Add(currentFrame);
		//Remove the last frame until we go below the max recording time
		while(currentFrame.Time - m_FrameHistory.Last().Time)
		{
			m_FrameHistory.PopFront();
		}
	}
	
	ShowFramePackage(currentFrame);
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& framePackage)
{
	for (const auto& hitBox : framePackage.HitBoxInfo)
	{
		DrawDebugBox(GetWorld(),
			hitBox.Value.Location,
			hitBox.Value.BoxExtend,
			hitBox.Value.Rotation.Quaternion(),
			FColor::Green,
			false,
			4.f);
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

