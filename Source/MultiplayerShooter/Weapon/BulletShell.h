// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletShell.generated.h"

class USoundCue;

USTRUCT()
struct FTimeAliveRange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Min = 2.0f;

	UPROPERTY(EditAnywhere)
	float Max = 5.0f;
};

UCLASS()
class MULTIPLAYERSHOOTER_API ABulletShell : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletShell();
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, DisplayName = "Mesh Component", Category = "Components")
	UStaticMeshComponent* m_pBulletShellMesh{};

	UPROPERTY(EditAnywhere, DisplayName = "Shell Ejection Force", Category = "Bullet Shell Properties")
	float m_ShellEjectionForce{ 15.0f };

	UPROPERTY(EditAnywhere, DisplayName = "Shell Drop Sound", Category = "Bullet Shell Properties")
	USoundCue* m_pShellDropSound{};

	UPROPERTY(EditAnywhere, Category = "Bullet Shell Properties", meta = (EditFixedSize, EditInline), DisplayName = "Time Alive Range")
	FTimeAliveRange m_TimeAliveRange;
	
	float m_MinTimeAlive{ 2.0f };
	float m_MaxTimeAlive{ 5.0f };

protected:
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
