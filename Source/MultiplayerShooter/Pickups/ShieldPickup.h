#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "ShieldPickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShieldPickup : public APickup
{
	GENERATED_BODY()
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Shield", DisplayName = "Shield Amount")
	float m_ShieldAmount{50.f};

	UPROPERTY(EditAnywhere, Category = "Shield", DisplayName = "Shield Time")
	float m_ShieldingTime{3.f};
};
