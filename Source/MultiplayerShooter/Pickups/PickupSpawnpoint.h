#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnpoint.generated.h"

class APickup;

UCLASS()
class MULTIPLAYERSHOOTER_API APickupSpawnpoint : public AActor
{
	GENERATED_BODY()

public:
	APickupSpawnpoint();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Pickup", DisplayName = "Pickup Classes")
	TArray<TSubclassOf<APickup>> m_PickupClasses{};

	void SpawnPickup();
	void SpawnPickupFinished();
	void StartSpawnTimer();
	UFUNCTION()
	void PickupDestroyed(AActor* destroyedActor);

	APickup* m_pCurrentPickup{nullptr};
private:
	FTimerHandle m_SpawnTimerHandle{};

	UPROPERTY(EditAnywhere, Category = "Pickup", DisplayName = "Min Spawn Delay")
	float m_MinSpawnDelay{ 5.f };
	UPROPERTY(EditAnywhere, Category = "Pickup", DisplayName = "Max Spawn Delay")
	float m_MaxSpawnDelay{ 10.f };
	
};
