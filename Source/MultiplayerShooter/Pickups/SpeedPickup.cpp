#include "SpeedPickup.h"

#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Character/Components/BuffComponent.h"

ASpeedPickup::ASpeedPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpeedPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpeedPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void ASpeedPickup::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	Super::OnSphereOverlap(overlappedComponent, otherActor, otherComponent, otherBodyIndex, bFromSweep, sweepResult);

	const ABlasterCharacter* pPlayer = Cast<ABlasterCharacter>(otherActor);
	if (!pPlayer) return;

	UBuffComponent* pBuffComponent = pPlayer->GetBuffComponent();
	if (!pBuffComponent) return;

	UE_LOG(LogTemp, Warning, TEXT("Speed buff time: %f"), m_SpeedBuffTime);
	pBuffComponent->BuffSpeed(m_BaseSpeedBuff, m_CrouchedSpeedBuff, m_SpeedBuffTime);

	Destroy();
}

