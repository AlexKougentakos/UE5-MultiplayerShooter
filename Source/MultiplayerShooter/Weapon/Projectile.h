#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

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
	UPROPERTY(EditAnywhere, DisplayName = "Projectile Collider")
	UBoxComponent* m_pCollisionBox{};

	UPROPERTY(EditAnywhere, DisplayName = "Projectile Movement Component")
	UProjectileMovementComponent* m_pProjectileMovementComponent{};
};
