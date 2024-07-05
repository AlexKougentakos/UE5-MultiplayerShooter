// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

class URocketMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;
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
		
	UPROPERTY(EditAnywhere, DisplayName = "Trail Effect", Category = "Effects")
	UNiagaraSystem* m_pTrailEffect{};

	UPROPERTY(EditAnywhere, DisplayName = "In Air Looped Sound", Category = "Effects")
	USoundCue* m_pProjectileInAirLoop{};

	UPROPERTY(EditAnywhere, DisplayName = "In Air Looped Sound Attenuation", Category = "Effects")
	USoundAttenuation* m_pProjectileInAirLoopAttenuation{};

	UPROPERTY(VisibleAnywhere, DisplayName = "Rocket Movement Component", Category = "Components")
	URocketMovementComponent* m_pRocketMovementComponent{};
private:
	UPROPERTY(VisibleAnywhere, DisplayName = "Rocket Mesh", Category = "Components")
	UStaticMeshComponent* m_pRocketMesh{};

	UNiagaraComponent* m_pTrailEffectComponent{};
	UAudioComponent* m_pProjectileInAirLoopComponent{};

	//How long to wait before destroying the rocket, in order to let the particle system play
	FTimerHandle m_DestructionTimer{};
	UPROPERTY(EditAnywhere, DisplayName = "Destroy Time", Category = "Effects")
	float m_DestroyTime{3.f};
	void DestroyTimerFinished();
};
