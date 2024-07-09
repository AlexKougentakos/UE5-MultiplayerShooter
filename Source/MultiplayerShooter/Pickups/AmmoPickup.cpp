#include "AmmoPickup.h"

#include "MultiplayerShooter/Character/BlasterCharacter.h"

AAmmoPickup::AAmmoPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	Super::OnSphereOverlap(overlappedComponent, otherActor, otherComponent, otherBodyIndex, bFromSweep, sweepResult);

	const ABlasterCharacter* pPlayer = Cast<ABlasterCharacter>(otherActor);
	if (!pPlayer) return;

	UCombatComponent* pCombatComponent = pPlayer->GetCombatComponent();
	if (!pCombatComponent) return;

	pCombatComponent->PickupAmmo(m_WeaponType, m_AmmoAmount);

	Destroy();
		
}

void AAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	checkf(m_pPickupMesh, TEXT("Pickup mesh is nullptr"));
	m_pPickupMesh->AddLocalRotation(FRotator(0.f, 45.f * DeltaTime, 0.f));
}

