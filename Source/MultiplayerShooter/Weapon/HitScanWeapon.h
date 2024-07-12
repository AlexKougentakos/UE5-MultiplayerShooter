// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void Fire(const FVector& hitTarget) override;
	
protected:
	UPROPERTY(EditAnywhere, DisplayName = "Damage", Category = "Weapon Stats")
	float m_Damage{20.f};

	UPROPERTY(EditAnywhere, DisplayName = "Impact Particles", Category = "Effects")
	UParticleSystem* m_pImpactParticles{};

	UPROPERTY(EditAnywhere, DisplayName = "Beam Particles", Category = "Effects")
	UParticleSystem* m_pBeamParticles{};
	
	void WeaponTraceHit(const FVector& traceStart, const FVector& hitTarget, FHitResult& outHitResult) const;

};
