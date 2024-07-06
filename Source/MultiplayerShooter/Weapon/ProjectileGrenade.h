#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileGrenade.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AProjectileGrenade : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileGrenade();
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
private:
	UFUNCTION()
	virtual void OnBounce(const FHitResult& HitResult, const FVector& ImpactVelocity);

	UPROPERTY(EditAnywhere, DisplayName = "Bounce Sound", Category = "Effects")
	USoundCue* m_pBounceSound{};
};
