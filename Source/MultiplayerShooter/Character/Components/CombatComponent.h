// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MultiplayerShooter/HUD/BlasterHUD.h"
#include "MultiplayerShooter/Types/CombatState.h"
#include "MultiplayerShooter/Weapon/WeaponTypes.h"
#include "CombatComponent.generated.h"


class ABlasterHUD;
class ABlasterPlayerController;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABlasterCharacter;
	
	virtual void TickComponent(float deltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void EquipWeapon(AWeapon* const pWeapon);
	bool HasWeapon() const { return m_pEquippedWeapon != nullptr; }
	bool HasSecondaryWeapon() const { return m_pSecondaryWeapon != nullptr; }
	void Reload();

	void FireButtonPressed(const bool isPressed);
	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();
	void JumpToShotgunReloadAnimationEnd() const;

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();

	void PickupAmmo(const EWeaponType weaponType, const int ammoAmount);

	void SwapWeapons();
	bool ShouldSwapWeapons() const;

	void ShowSniperScope(const bool showScope);

protected:
	virtual void BeginPlay() override;
	void SetAiming(const bool isAiming);
	void Fire();
	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgun();

	void TraceUnderCrosshairs(FHitResult& hitResult);
	
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(const bool isAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_SecondaryWeapon();
	
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& traceHitLocation);
	UFUNCTION(Server, Reliable)
	void ServerReload();
	UFUNCTION(Server, Reliable)
	void ShotgunServerFire(const TArray<FVector_NetQuantize>& traceHitLocations);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& traceHitLocation);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireShotgun(const TArray<FVector_NetQuantize>& traceHitLocations);

	void SetHudCrosshairs(float deltaTime);
	void UpdateWeaponHUD();

	bool CanFire() const;

	UFUNCTION(BlueprintCallable)
	void FinishedReloading();

	UFUNCTION(BlueprintCallable)
	void FinishedWeaponSwap();
	
	UFUNCTION(BlueprintCallable)
	void FinishedWeaponSwapAttachment();
	
	void ThrowGrenade();
	UFUNCTION(Server, Reliable)
	void ServerThrowGranade();

private:
	ABlasterCharacter* m_pCharacter{};
	ABlasterPlayerController* m_pPlayerController{};
	ABlasterHUD* m_pHud{};
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* m_pEquippedWeapon{};

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon* m_pSecondaryWeapon{};

	bool m_ShouldSwapWeapons{};

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool m_IsAiming{};
	bool m_IsAimButtonPressed{}; //This is used for the local player to determine if the aim button is pressed

	UFUNCTION()
	void OnRep_Aiming();

	bool m_IsFireButtonPressed{};
	bool m_IsLocallyReloading{};

	UPROPERTY(EditAnywhere, Category = Movement, DisplayName = "Base Walk Speed")
	float m_BaseWalkSpeed{};
	UPROPERTY(EditAnywhere, Category = Movement, DisplayName = "Aiming Walk Speed")
	float m_AimingWalkSpeed{};
	UPROPERTY(EditAnywhere, Category = Combat, DisplayName = "Crosshairs Center", meta = (ToolTip = "The amount the crosshair will move outwards when the player shoots 1 bullet"))
	float m_ShootingCrosshairSpread{1.f};
	
	float m_CrosshairInAirFactor{};
	float m_CrosshairAimFactor{};
	float m_CrosshairShootingFactor{};
	float m_CrosshairCrouchingFactor{};
	FHUDPackage m_HudPackage{};
	
	FVector m_HitTarget{};

	// Aiming
	float m_DefaultFOV{};
	UPROPERTY(EditAnywhere, Category = "Combat", DisplayName = "Zoomed FOV")
	float m_ZoomedFOV{30.f};

	float m_CurrentFOV{};
	
	UPROPERTY(EditAnywhere, Category = "Combat", DisplayName = "Zoom Interpolation Speed")
	float m_ZoomInterpolationSpeed{20.f};

	void InterpolateFOV(const float deltaTime);

	bool m_HideCrosshairs{false};

	// Automatic Fire
	FTimerHandle m_FireTimer{};
	bool m_CanFire{true};
	
	void StartFireTimer();
	void FireTimerFinished();

	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int m_CarriedAmmo{};

	UFUNCTION()
	void OnRep_CarriedAmmo();
	void LocalFire(const FVector_NetQuantize& traceHitLocation);
	void ShotgunLocalFire(const TArray<FVector_NetQuantize>& traceHitLocations);

	UPROPERTY(EditDefaultsOnly, Category = "Combat", DisplayName = "Carried Ammo Map")
	TMap<EWeaponType, int> m_CarriedAmmoMap{};

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState m_CombatState{ECombatState::ECS_Unoccupied};

	UFUNCTION()
	void OnRep_CombatState();

	// A function to hold common logic for reloading on both the server and client
	void HandleReloadingForBothServerAndClient();
	void UpdateAmmoValues(bool refreshHud);
	void UpdateShotgunAmmoValues();
	void UpdateAmmoHud();

	void EquipPrimaryWeapon(AWeapon* const pWeapon);
	void EquipSecondaryWeapon(AWeapon* const pWeapon);

	void AttachActorToBackpack(AActor* const pActor);
	void FireWeaponBasedOnFireType();
};
