#include "HealthPickup.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Character/Components/BuffComponent.h"


AHealthPickup::AHealthPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	m_pPickupFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Pickup FX"));
	m_pPickupFX->SetupAttachment(RootComponent);
}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHealthPickup::Destroyed()
{
	checkf(m_pPickupFXSystem, TEXT("Pickup FX System is nullptr"));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), m_pPickupFXSystem, GetActorLocation(), GetActorRotation());

	
	Super::Destroyed();
}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
                                    UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	Super::OnSphereOverlap(overlappedComponent, otherActor, otherComponent, otherBodyIndex, bFromSweep, sweepResult);

	Super::OnSphereOverlap(overlappedComponent, otherActor, otherComponent, otherBodyIndex, bFromSweep, sweepResult);

	const ABlasterCharacter* pPlayer = Cast<ABlasterCharacter>(otherActor);
	if (!pPlayer) return;

	UBuffComponent* pBuffComponent = pPlayer->GetBuffComponent();
	if (!pBuffComponent) return;
	
	pBuffComponent->Heal(m_HealAmount, m_HealingTime);
	

	Destroy();
}

