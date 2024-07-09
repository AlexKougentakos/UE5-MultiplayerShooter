#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "SpeedPickup.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API ASpeedPickup : public APickup
{
	GENERATED_BODY()

public:
	ASpeedPickup();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Speed Pickup", DisplayName = "Speed Buff")
	float m_BaseSpeedBuff{1600.f};

	UPROPERTY(EditAnywhere, Category = "Speed Pickup", DisplayName = "Crouched Speed Buff")
	float m_CrouchedSpeedBuff{ 850.f };

	UPROPERTY(EditAnywhere, Category = "Speed Pickup", DisplayName = "Speed Buff Time")
	float m_SpeedBuffTime{30.f};
};
