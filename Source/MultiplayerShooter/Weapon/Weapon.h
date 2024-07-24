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

UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScan UMETA(DisplayName = "HitScan"),
	EFT_Projectile UMETA(DisplayName = "Projectile"),
	EFT_Shotgun UMETA(DisplayName = "Shotgun"),
	
	EFT_MAX UMETA(DisplayName = "DefaultMAX")
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

	FVector GetVectorWithSpread(const FVector& hitTarget, float totalSpreadMultiplier) const;
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY() ABlasterCharacter* m_pWeaponHolder{};
	UPROPERTY() ABlasterPlayerController* m_pWeaponHolderController{};
		
	UPROPERTY(EditAnywhere, Category = "Effects", DisplayName = "Pickup Sound")
	USoundCue* m_pPickupSound{};
	
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

	UPROPERTY(EditAnywhere, Category = "Weapon Stats|Shooting", DisplayName = "Damage")
	float m_Damage{20.f};
	
	UPROPERTY(EditAnywhere, Category = "Weapon Stats|Shooting", DisplayName = "Headshot Damage")
	float m_HeadShotDamage{40.f};

	UPROPERTY(EditAnywhere, Category = "Weapon Stats|Shooting", DisplayName = "Fire Type")
	EFireType m_FireType{EFireType::EFT_HitScan};

		//Scatter
		UPROPERTY(EditAnywhere, DisplayName = "Use Scatter", Category = "Weapon Stats|Weapon Scatter", meta = (ToolTip = "If true, the weapon will use a random spread"))
		bool m_UseScatter{ false };
		
		UPROPERTY(EditAnywhere, DisplayName = "Spread Sphere Distance", Category = "Weapon Stats|Weapon Scatter", meta = (ToolTip = "The smaller the value, the bigger the spread"))
		float m_DistanceToSpreadSphere{ 800.f };
	
		float m_SphereRadius{ 50.f };

	
		UPROPERTY(EditAnywhere, DisplayName = "Base Spread", Category = "Weapon Stats|Weapon Scatter")
		float m_BaseSpread{ 0.5f };
		UPROPERTY(EditAnywhere, DisplayName = "Spread Increment", Category = "Weapon Stats|Weapon Scatter")
		float m_InAirFactor{ 1.5f };
		UPROPERTY(EditAnywhere, DisplayName = "Aim Factor", Category = "Weapon Stats|Weapon Scatter")
		float m_AimFactor{ -.5f };
		UPROPERTY(EditAnywhere, DisplayName = "Shooting Factor", Category = "Weapon Stats|Weapon Scatter")
		float m_CrouchingFactor{ -.5f };
		UPROPERTY(EditAnywhere, DisplayName = "Crouching Factor", Category = "Weapon Stats|Weapon Scatter")
		float m_MovingFactorMin{ 0.25f };
		UPROPERTY(EditAnywhere, DisplayName = "Moving Factor Max", Category = "Weapon Stats|Weapon Scatter")
		float m_MovingFactorMax{ 1.5f };
		UPROPERTY(EditAnywhere, DisplayName = "Shooting Factor", Category = "Weapon Stats|Weapon Scatter")
		float m_ShootingFactor{ .2f };
	
	UPROPERTY(Replicated, EditAnywhere, Category = "Server Side Rewind", DisplayName = "Use Server Side Rewind")
	bool m_UseServerSideRewind{false};

	UFUNCTION()
	void OnPingTooHigh(bool isPingTooHigh);
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

	UPROPERTY(EditAnywhere, Category = "Effects", DisplayName = "Killfeed Icon")
	UTexture2D* m_pKillfeedIcon{};

	UPROPERTY(EditAnywhere, Category = "Effects", DisplayName = "Weapon Holder Icon")
	UTexture2D* m_pWeaponHolderIcon{};

	/*
	 * AMMO
	 */
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties", DisplayName = "Weapon Ammo")
	int m_MaxAmmo{30};
	int m_CurrentAmmo{};
	void SpendAmmoRound();

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(const int serverAmmo);
	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(const int ammo);
	int m_AmmoSequence{}; // Used to keep track of the unprocessed server requests for ammo

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


	bool m_ShouldDestroyWeapon{false};

public:
	void SetWeaponState(const EWeaponState state);
	USkeletalMeshComponent* GetWeaponMesh() const { return m_pWeaponMesh; }
	float GetZoomedFOV() const { return m_ZoomedFOV; }
	float GetZoomInterpolationSpeed() const { return m_ZoomInterpolationSpeed; }

	float GetDamage() const { return m_Damage; }
	float GetHeadShotDamage() const { return m_HeadShotDamage; }
	float GetFireDelay() const { return m_FireDelay; }
	bool HasAutomaticFire() const { return m_CurrentWeaponHasAutomaticFire; }
	bool HasAmmoInMagazine() const { return m_CurrentAmmo > 0; }
	int GetCurrentAmmo() const { return m_CurrentAmmo; }
	bool IsMagazineFull() const { return m_CurrentAmmo == m_MaxAmmo; }
	EWeaponType GetWeaponType() const { return m_WeaponType; }
	EFireType GetFireType() const { return m_FireType; }
	USoundCue* GetPickupSound() const { return m_pPickupSound; }
	UTexture2D* GetKillfeedIcon() const { return m_pKillfeedIcon; }
	UTexture2D* GetWeaponHolderIcon() const { return m_pWeaponHolderIcon; }
	bool ShouldDestroyWeapon() const { return m_ShouldDestroyWeapon; }
	void SetShouldDestroyWeapon(const bool shouldDestroy) { m_ShouldDestroyWeapon = shouldDestroy; }
	bool UseScatter() const { return m_UseScatter; }
	EWeaponState GetWeaponState() const { return m_WeaponState; }

	//Spread Getters
	float GetBaseSpread() const { return m_BaseSpread; }
	float GetDistanceToSpreadSphere() const { return m_DistanceToSpreadSphere; }
	float GetSphereRadius() const { return m_SphereRadius; }
	float GetInAirFactor() const { return m_InAirFactor; }
	float GetAimFactor() const { return m_AimFactor; }
	float GetCrouchingFactor() const { return m_CrouchingFactor; }
	float GetMovingFactorMin() const { return m_MovingFactorMin; }
	float GetMovingFactorMax() const { return m_MovingFactorMax; }
	float GetShootingFactor() const { return m_ShootingFactor; }
};
