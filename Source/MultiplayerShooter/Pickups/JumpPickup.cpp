#include "JumpPickup.h"

#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Character/Components/BuffComponent.h"


AJumpPickup::AJumpPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AJumpPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void AJumpPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AJumpPickup::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	Super::OnSphereOverlap(overlappedComponent, otherActor, otherComponent, otherBodyIndex, bFromSweep, sweepResult);

	const ABlasterCharacter* pPlayer = Cast<ABlasterCharacter>(otherActor);
	if (!pPlayer) return;

	UBuffComponent* pBuffComponent = pPlayer->GetBuffComponent();
	if (!pBuffComponent) return;
	
	pBuffComponent->BuffJump(m_JumpBuff, m_JumpBuffTime);

	Destroy();
}

