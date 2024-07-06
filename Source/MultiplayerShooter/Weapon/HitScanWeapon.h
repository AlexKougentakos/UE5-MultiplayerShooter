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
private:

	UPROPERTY(EditAnywhere, DisplayName = "Spread Sphere Distance", Category = "Weapon Stats|Weapon Scatter", meta = (ToolTip = "The smaller the value, the bigger the spread"))
	float m_DistanceToSpreadSphere{ 800.f };

	UPROPERTY(EditAnywhere, DisplayName = "Sphere Radius", Category = "Weapon Stats|Weapon Scatter", meta = (ToolTip = "The bigger the value, the bigger the spread"))
	float m_SphereRadius{ 75.f };

	UPROPERTY(EditAnywhere, DisplayName = "Use Scatter", Category = "Weapon Stats|Weapon Scatter", meta = (ToolTip = "If true, the weapon will use a random spread"))
	bool m_UseScatter{ false };
	
protected:
	UPROPERTY(EditAnywhere, DisplayName = "Damage", Category = "Weapon Stats")
	float m_Damage{20.f};

	UPROPERTY(EditAnywhere, DisplayName = "Impact Particles", Category = "Effects")
	UParticleSystem* m_pImpactParticles{};

	UPROPERTY(EditAnywhere, DisplayName = "Beam Particles", Category = "Effects")
	UParticleSystem* m_pBeamParticles{};

	FVector GetVectorWithSpread(const FVector& hitStart, const FVector& hitTarget) const;
};
