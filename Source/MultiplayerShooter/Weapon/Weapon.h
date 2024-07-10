// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USoundCue;
class ABlasterPlayerController;
class ABlasterCharacter;
class ABulletShell;
class UWidgetComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "InitialState"),
	EWS_Equipped UMETA(DisplayName = "EquippedState"),
	EWS_EquippedSecondary UMETA(DisplayName = "EquippedSecondaryState"),
	EWS_Dropped UMETA(DisplayName = "DroppedState"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
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

	/**
	 * 
	 * @param availableAmmo the available ammo that you can offer for a reload
	 * @return the amount of ammo used for the reload
	 */
	virtual int Reload(const int availableAmmo);
	void Drop();

	void OnRep_Owner() override;
	void UpdateHudAmmo();

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
	
	void EnableCustomDepth(bool enable) const;
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


	// Zooming
	UPROPERTY(EditAnywhere, Category = "Weapon Stats|Zooming", DisplayName = "Zoomed FOV")
	float m_ZoomedFOV{30.f};

	UPROPERTY(EditAnywhere, Category = "Weapon Stats|Zooming", DisplayName = "Zoom Interpolation Speed")
	float m_ZoomInterpolationSpeed{20.f};

	//Weapon Stats
	UPROPERTY(EditAnywhere, Category = "Weapon Stats|Shooting", DisplayName = "Fire Delay")
	float m_FireDelay{.15f};

	UPROPERTY(EditAnywhere, Category = "Weapon Stats|Shooting", DisplayName = "Has Automatic Fire")
	bool m_CurrentWeaponHasAutomaticFire{true};
	
	UPROPERTY(EditAnywhere, Category = "Effects", DisplayName = "Pickup Sound")
	USoundCue* m_pPickupSound{};



private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", DisplayName = "Weapon Mesh")
	USkeletalMeshComponent* m_pWeaponMesh{};

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", DisplayName = "Pickup Area Sphere")
	USphereComponent* m_pAreaSphere{};

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties",
		DisplayName = "Weapon State")
	EWeaponState m_WeaponState{EWeaponState::EWS_Initial};

	UPROPERTY(EditAnywhere, Category = "Weapon Properties", DisplayName = "Weapon Type")
	EWeaponType m_WeaponType{EWeaponType::EWT_Rifle};

	/*
	 * AMMO
	 */
	UPROPERTY(EditAnywhere, Category = "Weapon Properties", DisplayName = "Weapon Ammo")
	int m_MaxAmmo{30};
	UPROPERTY(ReplicatedUsing = OnRep_Ammo)
	int m_CurrentAmmo{};
	UFUNCTION()
	void OnRep_Ammo();
	void SpendAmmoRound();


	UFUNCTION()
	void OnRep_WeaponState();

	void OnEquipped();
	void OnEquippedSecondary();
	void OnDropped();
	

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", DisplayName = "Pick Up Widget")
	UWidgetComponent* m_pPickUpWidget{};

	UPROPERTY(EditAnywhere, Category = "Effects", DisplayName = "Fire Animation")
	UAnimationAsset* m_pFireAnimation;

	UPROPERTY(EditAnywhere, Category = "Effects", DisplayName = "Bullet Shell Class")
	TSubclassOf<ABulletShell> m_pBulletShellClass{};

	ABlasterCharacter* m_pWeaponHolder{};
	ABlasterPlayerController* m_pWeaponHolderController{};

	bool m_ShouldDestroyWeapon{false};
public:
	void SetWeaponState(const EWeaponState state);
	USkeletalMeshComponent* GetWeaponMesh() const { return m_pWeaponMesh; }
	float GetZoomedFOV() const { return m_ZoomedFOV; }
	float GetZoomInterpolationSpeed() const { return m_ZoomInterpolationSpeed; }

	float GetFireDelay() const { return m_FireDelay; }
	bool HasAutomaticFire() const { return m_CurrentWeaponHasAutomaticFire; }
	bool HasAmmoInMagazine() const { return m_CurrentAmmo > 0; }
	int GetCurrentAmmo() const { return m_CurrentAmmo; }
	bool IsMagazineFull() const { return m_CurrentAmmo == m_MaxAmmo; }
	EWeaponType GetWeaponType() const { return m_WeaponType; }
	USoundCue* GetPickupSound() const { return m_pPickupSound; }
	bool ShouldDestroyWeapon() const { return m_ShouldDestroyWeapon; }
	void SetShouldDestroyWeapon(const bool shouldDestroy) { m_ShouldDestroyWeapon = shouldDestroy; }
};
