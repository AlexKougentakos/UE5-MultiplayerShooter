#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USoundCue;
class USphereComponent;

UCLASS()
class MULTIPLAYERSHOOTER_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();
	
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void Tick(float DeltaTime) override;
protected:
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UPROPERTY(EditAnywhere, Category = "Components", DisplayName = "Pickup Mesh")
	UStaticMeshComponent* m_pPickupMesh{};
		
	UPROPERTY(EditAnywhere, Category = "Effects", DisplayName = "Pickup FX System")
	UNiagaraSystem* m_pPickupFXSystem{};
	
	UPROPERTY(VisibleAnywhere, Category = "Effects", DisplayName = "Pickup FX")
	UNiagaraComponent* m_pPickupFX{};
private:
	UPROPERTY(EditAnywhere, Category = "Components", DisplayName = "Overlap Sphere")
	USphereComponent* m_pSphereComponent{};

	UPROPERTY(EditAnywhere, Category = "Effects", DisplayName = "Pickup Sound")
	USoundCue* m_pPickupSound{};

	FTimerHandle m_DelayOverlapBindingHandle{};
	void DelayOverlapBinding();

};
