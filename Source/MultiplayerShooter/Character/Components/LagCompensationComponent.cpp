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

	FFramePackage framePackage{};
	SaveFramePackage(framePackage);
	ShowFramePackage(framePackage);
}


void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
			true);
	}
}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& framePackage)
{
	checkf(m_pCharacter, TEXT("Character is nullptr"));

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

