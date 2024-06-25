// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void Fire(const FVector& hitTarget) override;
private:
	UPROPERTY(EditAnywhere, Category = "Projectile Weapon Properties", DisplayName = "Projectile Class")
	TSubclassOf<AProjectile> m_pProjectileClass;
};
