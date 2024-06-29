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

private:

	UPROPERTY(EditAnywhere, DisplayName = "Projectile Collider", Category = "Components")
	UBoxComponent* m_pCollisionBox{};

	UPROPERTY(EditAnywhere, DisplayName = "Projectile Movement Component", Category = "Components")
	UProjectileMovementComponent* m_pProjectileMovementComponent{};

	UPROPERTY(EditAnywhere, DisplayName = "Projectile Tracer Effect", Category = "Effects")
	UParticleSystem* m_pTracerEffect{};

	UPROPERTY(EditAnywhere, DisplayName = "Projectile Impact Sound", Category = "Effects")
	USoundCue* m_pImpactSound{};

	UParticleSystemComponent* m_pParticleSystemComponent{};

	UPROPERTY(EditAnywhere, DisplayName = "Grass Impact Effect", Category = "Effects|Impact Particles")
	UParticleSystem* m_pGrassImpactEffect{};
	
	UPROPERTY(EditAnywhere, DisplayName = "Wood Impact Effect", Category = "Effects|Impact Particles")
	UParticleSystem* m_pWoodImpactEffect{};

	UPROPERTY(EditAnywhere, DisplayName = "Player Impact Effect", Category = "Effects|Impact Particles")
	UParticleSystem* m_pPlayerImpactEffect{};
	
	UPROPERTY(EditAnywhere, DisplayName = "Rock Impact Effect", Category = "Effects|Impact Particles")
	UParticleSystem* m_pRockImpactEffect{};
	
	UPROPERTY(EditAnywhere, DisplayName = "Metal Impact Effect", Category = "Effects|Impact Particles")
	UParticleSystem* m_pMetalImpactEffect{};


	

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

protected:
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHit(const UPhysicalMaterial* physicalMaterial);
	
	virtual UParticleSystem* GetImpactEffect(const UPhysicalMaterial* physicalMaterial) const;

	UPROPERTY(EditAnywhere, DisplayName = "Damage", Category = "Projectile")
	float m_Damage{ 20.0f };

};
