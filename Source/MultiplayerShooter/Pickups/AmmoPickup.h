#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "MultiplayerShooter/Weapon/WeaponTypes.h"
#include "AmmoPickup.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

public:
	AAmmoPickup();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Ammo Pickup", DisplayName = "Ammo Amount")
	int m_AmmoAmount{ 10 };

	UPROPERTY(EditAnywhere, Category = "Ammo Pickup", DisplayName = "Ammo Type")
	EWeaponType m_WeaponType{ EWeaponType::EWT_Rifle };
public:
};
