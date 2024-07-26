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

	UPROPERTY(EditAnywhere, DisplayName = "Impact Particles", Category = "Effects")
	UParticleSystem* m_pImpactParticles{};

	UPROPERTY(EditAnywhere, DisplayName = "Beam Particles", Category = "Effects")
	UParticleSystem* m_pBeamParticles{};
	
	void WeaponTraceHit(const FVector& traceStart, const FVector& hitTarget, FHitResult& outHitResult) const;

	UPROPERTY(EditAnywhere, DisplayName = "Grass Impact Effect", Category = "Effects|Impact")
	UParticleSystem* m_pGrassImpactEffect{};
	
	UPROPERTY(EditAnywhere, DisplayName = "Wood Impact Effect", Category = "Effects|Impact")
	UParticleSystem* m_pWoodImpactEffect{};

	UPROPERTY(EditAnywhere, DisplayName = "Player Impact Effect", Category = "Effects|Impact")
	UParticleSystem* m_pPlayerImpactEffect{};
	
	UPROPERTY(EditAnywhere, DisplayName = "Rock Impact Effect", Category = "Effects|Impact")
	UParticleSystem* m_pRockImpactEffect{};
	
	UPROPERTY(EditAnywhere, DisplayName = "Metal Impact Effect", Category = "Effects|Impact")
	UParticleSystem* m_pMetalImpactEffect{};

	UPROPERTY(EditAnywhere, DisplayName = "Projectile Impact Sound", Category = "Effects|Impact")
	USoundCue* m_pImpactSound{};
	
	UPROPERTY(EditAnywhere, DisplayName = "Player Impact Sound", Category = "Effects|Impact")
	USoundCue* m_pPlayerImpactSound{};

	UPROPERTY(EditAnywhere, DisplayName = "Player Impact Sound", Category = "Effects|Impact")
	USoundCue* m_pHeadshotImpactSound{};
private:
	UPROPERTY(EditAnywhere, DisplayName = "Grass Physical Material", Category = "Effects|Physical Materials")
	UPhysicalMaterial* m_pGrassPhysicalMaterial{};

	UPROPERTY(EditAnywhere, DisplayName = "Wood Physical Material", Category = "Effects|Physical Materials")
	UPhysicalMaterial* m_pWoodPhysicalMaterial{};

	UPROPERTY(EditAnywhere, DisplayName = "Player Physical Material", Category = "Effects|Physical Materials")
	UPhysicalMaterial* m_pPlayerPhysicalMaterial{};

	UPROPERTY(EditAnywhere, DisplayName = "Metal Physical Material", Category = "Effects|Physical Materials")
	UPhysicalMaterial* m_pMetalPhysicalMaterial{};

	UPROPERTY(EditAnywhere, DisplayName = "Rock Physical Material", Category = "Effects|Physical Materials")
	UPhysicalMaterial* m_pRockPhysicalMaterial{};

	UParticleSystem* GetImpactEffect(const UPhysicalMaterial* physicalMaterial) const;
	const UPhysicalMaterial* GetMaterialOfActor(AActor* OtherActor) const;
};
