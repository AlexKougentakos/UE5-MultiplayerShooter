#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "HealthPickup.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class MULTIPLAYERSHOOTER_API AHealthPickup : public APickup
{
	GENERATED_BODY()

public:
	AHealthPickup();

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult) override;

private:

	UPROPERTY(EditAnywhere, Category = "Health Pickup", DisplayName = "Heal Amount")
	float m_HealAmount{ 50.f };

	UPROPERTY(EditAnywhere, Category = "Health Pickup", DisplayName = "Heal Time", meta = (ToolTip = "The amount of times it takes to apply 100 health"))
	float m_HealingTime{ 3.f };

};
