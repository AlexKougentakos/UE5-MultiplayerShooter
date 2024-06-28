// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class ABulletShell;
class UWidgetComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial 	UMETA(DisplayName = "InitialState"),
	EWS_Equipped 	UMETA(DisplayName = "EquippedState"),
	EWS_Dropped 	UMETA(DisplayName = "DroppedState"),

	EWS_MAX 		UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class MULTIPLAYERSHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(bool show) const;

	virtual void Fire(const FVector& hitTarget);

	/*
	 * Textures for the weapon crosshairs
	 */

	UPROPERTY(EditAnywhere, Category = "Crosshairs", DisplayName = "Crosshairs Center")
	UTexture2D* m_pCrosshairsCenter{};

	UPROPERTY(EditAnywhere, Category = "Crosshairs", DisplayName = "Crosshairs Top")
	UTexture2D* m_pCrosshairsTop{};

	UPROPERTY(EditAnywhere, Category = "Crosshairs", DisplayName = "Crosshairs Right")
	UTexture2D* m_pCrosshairsRight{};

	UPROPERTY(EditAnywhere, Category = "Crosshairs", DisplayName = "Crosshairs Bottom")
	UTexture2D* m_pCrosshairsBottom{};

	UPROPERTY(EditAnywhere, Category = "Crosshairs", DisplayName = "Crosshairs Left")
	UTexture2D* m_pCrosshairsLeft{};

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex
		, bool bFromSweep
		, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereOverlapEnd(
		UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex);
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", DisplayName = "Weapon Mesh")
	USkeletalMeshComponent* m_pWeaponMesh{};

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", DisplayName = "Pickup Area Sphere")
	USphereComponent* m_pAreaSphere{};

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties", DisplayName = "Weapon State")
	EWeaponState m_WeaponState{ EWeaponState::EWS_Initial };

	UFUNCTION()
	void OnRep_WeaponState() const;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", DisplayName = "Pick Up Widget")
	UWidgetComponent* m_pPickUpWidget{};

	UPROPERTY(EditAnywhere, Category = "Weapon FX", DisplayName = "Fire Animation")
	UAnimationAsset* m_pFireAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon FX", DisplayName = "Bullet Shell Class")
	TSubclassOf<ABulletShell> m_pBulletShellClass{};
public:
	void SetWeaponState(const EWeaponState state);
	USkeletalMeshComponent* GetWeaponMesh() const { return m_pWeaponMesh; }
};
