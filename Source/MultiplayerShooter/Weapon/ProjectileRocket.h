// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

class URocketMovementComponent;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileRocket();
	virtual void Destroyed() override {}; //To avoid calling the base class implementation since it will play the effects again
protected:
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, DisplayName = "In Air Looped Sound", Category = "Effects")
	USoundCue* m_pProjectileInAirLoop{};

	UPROPERTY(EditAnywhere, DisplayName = "In Air Looped Sound Attenuation", Category = "Effects")
	USoundAttenuation* m_pProjectileInAirLoopAttenuation{};

	UPROPERTY(VisibleAnywhere, DisplayName = "Rocket Movement Component", Category = "Components")
	URocketMovementComponent* m_pRocketMovementComponent{};
private:

	UAudioComponent* m_pProjectileInAirLoopComponent{};
};
