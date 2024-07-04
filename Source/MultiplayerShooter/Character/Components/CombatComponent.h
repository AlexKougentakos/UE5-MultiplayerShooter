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
	void Reload();

	void FireButtonPressed(const bool isPressed);
protected:
	virtual void BeginPlay() override;
	void SetAiming(const bool isAiming);
	void Fire();

	void TraceUnderCrosshairs(FHitResult& hitResult);
	
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(const bool isAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& traceHitLocation);
	UFUNCTION(Server, Reliable)
	void ServerReload();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& traceHitLocation);

	void SetHudCrosshairs(float deltaTime);

	bool CanFire() const;

	UFUNCTION(BlueprintCallable)
	void FinishedReloading();

private:
	ABlasterCharacter* m_pCharacter{};
	ABlasterPlayerController* m_pPlayerController{};
	ABlasterHUD* m_pHud{};
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* m_pEquippedWeapon{};

	UPROPERTY(Replicated)
	bool m_IsAiming{};

	bool m_IsFireButtonPressed{};

	UPROPERTY(EditAnywhere, Category = Movement, DisplayName = "Base Walk Speed")
	float m_BaseWalkSpeed{};

	UPROPERTY(EditAnywhere, Category = Movement, DisplayName = "Aiming Walk Speed")
	float m_AimingWalkSpeed{};

	UPROPERTY(EditAnywhere, Category = Combat, DisplayName = "Crosshairs Center", meta = (ToolTip = "The amount the crosshair will move outwards when the player shoots 1 bullet"))
	float m_ShootingCrosshairSpread{1.f};
	
	float m_CrosshairInAirFactor{};
	float m_CrosshairAimFactor{};
	float m_CrosshairShootingFactor{};
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

	// Automatic Fire
	FTimerHandle m_FireTimer{};
	bool m_CanFire{true};
	
	void StartFireTimer();
	void FireTimerFinished();

	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int m_CarriedAmmo{};

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int> m_CarriedAmmoMap{};
	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState m_CombatState{ECombatState::ECS_Unoccupied};

	UFUNCTION()
	void OnRep_CombatState();

	// A function to hold common logic for reloading on both the server and client
	void HandleReloadingForBothServerAndClient();
	void UpdateAmmoValues();
	
};
