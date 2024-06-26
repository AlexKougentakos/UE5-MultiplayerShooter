#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USoundCue;
class UProjectileMovementComponent;
class UBoxComponent;

UCLASS()
class MULTIPLAYERSHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
private:
	UPROPERTY(EditAnywhere, DisplayName = "Projectile Collider", Category = "Components")
	UBoxComponent* m_pCollisionBox{};

	UPROPERTY(EditAnywhere, DisplayName = "Projectile Movement Component", Category = "Components")
	UProjectileMovementComponent* m_pProjectileMovementComponent{};

	UPROPERTY(EditAnywhere, DisplayName = "Projectile Tracer Effect", Category = "Effects")
	UParticleSystem* m_pTracerEffect{};

	UPROPERTY(EditAnywhere, DisplayName = "Projectile Impact Effect", Category = "Effects")
	UParticleSystem* m_pImpactEffect{};

	UPROPERTY(EditAnywhere, DisplayName = "Projectile Impact Sound", Category = "Effects")
	USoundCue* m_pImpactSound{};

	UParticleSystemComponent* m_pParticleSystemComponent{};

	

protected:
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
