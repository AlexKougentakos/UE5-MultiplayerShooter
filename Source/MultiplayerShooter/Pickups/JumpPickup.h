// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "JumpPickup.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AJumpPickup : public APickup
{
	GENERATED_BODY()

public:
	AJumpPickup();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Jump Pickup", DisplayName = "Jump Buff")
	float m_JumpBuff{ 4000.f };

	UPROPERTY(EditAnywhere, Category = "Jump Pickup", DisplayName = "Jump Buff Time")
	float m_JumpBuffTime{ 30.f };
};
