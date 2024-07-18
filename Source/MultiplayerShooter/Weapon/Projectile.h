#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
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
	UPROPERTY(EditAnywhere, DisplayName = "Projectile Tracer Effect", Category = "Effects")
	UParticleSystem* m_pTracerEffect{};
	
	UParticleSystemComponent* m_pParticleSystemComponent{};

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

	FTimerHandle m_DestructionTimer{};
	UPROPERTY(EditAnywhere, DisplayName = "Destroy Time", Category = "Effects")
	float m_DestroyTime{3.f};

protected:

	//This must be constructed on each class, in case you need to use a custom movement component
	UPROPERTY(EditAnywhere, DisplayName = "Projectile Movement Component", Category = "Components")
	UProjectileMovementComponent* m_pProjectileMovementComponent{};

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHit(const UPhysicalMaterial* physicalMaterial);
	
	virtual UParticleSystem* GetImpactEffect(const UPhysicalMaterial* physicalMaterial) const;

	const UPhysicalMaterial* GetMaterialOfActor(AActor* OtherActor) const;

	void SpawnTrailSystem();
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void DoDamageWithFallOff(const float innerDamageRadius, const float outerDamageRadius, const float damageAmount);
		
	UPROPERTY(EditAnywhere, Category = "Projectile", DisplayName = "Use Server Side Rewind")
	bool m_UseServerSideRewind = false;
	
	FVector_NetQuantize m_SpawnLocation{};
	FVector_NetQuantize100 m_InitialVelocity{};
	float m_Damage{ 20.0f };

	UPROPERTY(EditAnywhere, DisplayName = "Damage Fall Off Inner Radius", Category = "Projectile Stats")
	float m_InnerDamageRadius{ 200.0f };

	UPROPERTY(EditAnywhere, DisplayName = "Damage Fall Off Outer Radius", Category = "Projectile Stats")
	float m_OuterDamageRadius{ 500.0f };

	UPROPERTY(EditAnywhere, DisplayName = "Projectile Collider", Category = "Components")
	UBoxComponent* m_pCollisionBox{};

	UPROPERTY(VisibleAnywhere, DisplayName = "Projectile Mesh", Category = "Components")
	UStaticMeshComponent* m_pProjectileMesh{};
	
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


	UPROPERTY(EditAnywhere, DisplayName = "Trail Effect", Category = "Effects")
	UNiagaraSystem* m_pTrailEffect{};
	UPROPERTY() UNiagaraComponent* m_pTrailEffectComponent{};
public:
	void SetServerSideRewind(const bool useServerSideRewind) { m_UseServerSideRewind = useServerSideRewind; }
	void SetInitialVelocity(const FVector& initialVelocity) { m_InitialVelocity = initialVelocity; }
	void SetSpawnLocation(const FVector& spawnLocation) { m_SpawnLocation = spawnLocation; }
	const UProjectileMovementComponent* GetProjectileMovementComponent() const { return m_pProjectileMovementComponent; }
	void SetDamage(const float damage) { m_Damage = damage; }
};