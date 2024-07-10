#include "PickupSpawnpoint.h"

#include "Pickup.h"

APickupSpawnpoint::APickupSpawnpoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void APickupSpawnpoint::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnTimer();
}

void APickupSpawnpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupSpawnpoint::SpawnPickup()
{
	checkf(m_PickupClasses.Num() > 0, TEXT("No pickup classes set in %s"), *GetName());
	const int randomIndex = FMath::RandRange(0, m_PickupClasses.Num() - 1);
	const TSubclassOf<APickup> pickupClass = m_PickupClasses[randomIndex];
	if (!pickupClass) return;
	
	m_pCurrentPickup = GetWorld()->SpawnActor<APickup>(pickupClass, GetActorTransform());
	if (!m_pCurrentPickup) return;

	if (HasAuthority())
		m_pCurrentPickup->OnDestroyed.AddDynamic(this, &APickupSpawnpoint::PickupDestroyed);
}

void APickupSpawnpoint::SpawnPickupFinished()
{
	if (HasAuthority())
	{
		SpawnPickup();
	}
}

void APickupSpawnpoint::StartSpawnTimer()
{
	const float spawnTime = FMath::RandRange(m_MinSpawnDelay, m_MaxSpawnDelay);
	GetWorldTimerManager().SetTimer(m_SpawnTimerHandle, this, &APickupSpawnpoint::SpawnPickupFinished, spawnTime, false);
}

void APickupSpawnpoint::PickupDestroyed(AActor* destroyedActor)
{
	StartSpawnTimer();
}

