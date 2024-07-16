// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiplayerShooter/Types/TurningInPlace.h"
#include "Components/CombatComponent.h"
#include "Components/TimelineComponent.h"
#include "MultiplayerShooter/Interfaces/InteractWithCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"

class ULagCompensationComponent;
class UBoxComponent;
class UBuffComponent;
//Forward declarations
class ABlasterPlayerState;
class USoundCue;
class UCombatComponent;
class AWeapon;
class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class MULTIPLAYERSHOOTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Jump() override;
	virtual void Destroyed() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminated();
	void Eliminated();

	void PlayFireMontage(const bool isAiming) const;
	void PlayHitReactMontage() const;
	void PlayEliminationMontage() const;
	void PlayRifleReloadMontage() const;
	void PlayThrowGrenadeMontage() const;
	void PlayWeaponSwapMontage() const;
	
	UFUNCTION(NetMulticast, Unreliable) //Unreliable because it's a cosmetic effect that will mostly go unnoticed with so many hits
	void MulticastHit();

	virtual void OnRep_ReplicatedMovement() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool showScope);

	void UpdateHudHealth();
	void UpdateHudShield();
	void UpdateHudAmmo();

	void SpawnDefaultWeapon();

	UPROPERTY(EditAnywhere, DisplayName = "Head Box", Category = "Combat|HitBoxes")
	UBoxComponent* head{};
	UPROPERTY(EditAnywhere, DisplayName = "Pelvis Box", Category = "Combat|HitBoxes")
	UBoxComponent* pelvis{};
	UPROPERTY(EditAnywhere, DisplayName = "Spine 01 Box", Category = "Combat|HitBoxes")
	UBoxComponent* spine_01{};
	UPROPERTY(EditAnywhere, DisplayName = "Spine 02 Box", Category = "Combat|HitBoxes")
	UBoxComponent* spine_02{};
	UPROPERTY(EditAnywhere, DisplayName = "Upper Arm L Box", Category = "Combat|HitBoxes")
	UBoxComponent* upperarm_l{};
	UPROPERTY(EditAnywhere, DisplayName = "Upper Arm R Box", Category = "Combat|HitBoxes")
	UBoxComponent* upperarm_r{};
	UPROPERTY(EditAnywhere, DisplayName = "Lower Arm L Box", Category = "Combat|HitBoxes")
	UBoxComponent* lowerarm_l{};
	UPROPERTY(EditAnywhere, DisplayName = "Lower Arm R Box", Category = "Combat|HitBoxes")
    UBoxComponent* lowerarm_r{};
	UPROPERTY(EditAnywhere, DisplayName = "Hand L Box", Category = "Combat|HitBoxes")
	UBoxComponent* hand_l{};
	UPROPERTY(EditAnywhere, DisplayName = "Hand R Box", Category = "Combat|HitBoxes")
	UBoxComponent* hand_r{};
	UPROPERTY(EditAnywhere, DisplayName = "Backpack Box", Category = "Combat|HitBoxes")
	UBoxComponent* backpack{};
	UPROPERTY(EditAnywhere, DisplayName = "Thigh L Box", Category = "Combat|HitBoxes")
	UBoxComponent* thigh_l{};
	UPROPERTY(EditAnywhere, DisplayName = "Thigh R Box", Category = "Combat|HitBoxes")
	UBoxComponent* thigh_r{};
	UPROPERTY(EditAnywhere, DisplayName = "Calf L Box", Category = "Combat|HitBoxes")
	UBoxComponent* calf_l{};
	UPROPERTY(EditAnywhere, DisplayName = "Calf R Box", Category = "Combat|HitBoxes")
	UBoxComponent* calf_r{};
	UPROPERTY(EditAnywhere, DisplayName = "Foot L Box", Category = "Combat|HitBoxes")
	UBoxComponent* foot_l{};
	UPROPERTY(EditAnywhere, DisplayName = "Foot R Box", Category = "Combat|HitBoxes")
	UBoxComponent* foot_r{};

	UPROPERTY()
	TMap<FName, UBoxComponent*> HitBoxes{};
	
private: // Variables
	UPROPERTY(VisibleAnywhere, Category = Camera, DisplayName = "Camera Boom")
	USpringArmComponent* m_pCameraBoom{};

	UPROPERTY(VisibleAnywhere, Category = Camera, DisplayName = "Follow Camera")
	UCameraComponent* m_pFollowCamera{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), DisplayName = "Overhead Widget")
	UWidgetComponent* m_pOverheadWidget{};

	UPROPERTY(VisibleAnywhere, DisplayName = "Combat Component", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* m_pCombat{};
	
	UPROPERTY(VisibleAnywhere, DisplayName = "Lag Compensation Component", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ULagCompensationComponent* m_pLagCompensation{};

	UPROPERTY(VisibleAnywhere, DisplayName = "Buff Component", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBuffComponent* m_pBuffComponent{};

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* m_pOverlappingWeapon{};

	float m_AimOffsetPitch{};
	float m_AimOffsetYaw{};
	float m_InterpolatedAimOffsetYaw{};

	bool m_PollInitializedComplete{};
	float m_ElapsedPollingTime{0.f};
	const float m_MaxPollingTime{3.f};

	bool m_FinishedSwappingWeapons{};

	FRotator m_LastFrameRotation{};

	ABlasterPlayerState* m_pBlasterPlayerState{};
	
	ETurningInPlace m_TurningInPlace{};
	UPROPERTY(EditAnywhere, Category = "Movement", DisplayName = "Player Rotation Angle Standing", meta = (ToolTip = "The angle at which the player rotates when turning in place when standing"))
	float m_StandingPlayerRotationAngle{80.f};
	UPROPERTY(EditAnywhere, Category = "Movement", DisplayName = "Player Rotation Angle Crouched", meta = (ToolTip = "The angle at which the player rotates when turning in place when crouched"))
	float m_CrouchingPlayerRotationAngle{60.f};
	UPROPERTY(EditAnywhere, Category = "Movement", DisplayName = "Player Rotation Speed")
	float m_RotationSpeed{5.f};
	
	UPROPERTY(EditAnywhere, DisplayName = "Fire Weapon Montage", Category = "Animation")
	UAnimMontage* m_pFireWeaponMontage{};

	UPROPERTY(EditAnywhere, DisplayName = "Hit React Montage", Category = "Animation")
	UAnimMontage* m_pHitReactMontage{};

	UPROPERTY(EditAnywhere, DisplayName = "EliminationReact Montage", Category = "Animation")
	UAnimMontage* m_pEliminationMontage{};
	
	UPROPERTY(EditAnywhere, DisplayName = "Rifle Reload Montage", Category = "Animation")
	UAnimMontage* m_pReloadMontage{};

	UPROPERTY(EditAnywhere, DisplayName = "Grenade Throw Montage", Category = "Animation")
	UAnimMontage* m_pGrenadeThrowMontage{};

	UPROPERTY(EditAnywhere, DisplayName = "Weapon Swap Montage", Category = "Animation")
	UAnimMontage* m_pWeaponSwapMontage{};

	
	void HideCameraWhenPlayerIsClose();
	UPROPERTY(EditAnywhere, DisplayName = "Player Hide Distance", meta = (ToolTip = "The minimum distance there needs to be between the player and the camera for the camera to remain active"))
	float m_PlayerHideDistance{200.f}; // The minimum distance there needs to be between the player and the camera for the camera to remain active

	/*
	 * PROXY SIMULATED TURNING
	 */
	bool m_RotateRootBone{};
	float m_TurnThreshold{2.f};
	FRotator m_ProxyRotationLastFrame{};
	FRotator m_CurrentProxyRotation{};
	float m_TimeSinceLastMovementReplication{};

	/*
	 * Player Health & Shield
	 */

	UPROPERTY(EditAnywhere, DisplayName = "Max Health", Category = "Player Stats")
	float m_MaxHealth{100.f};

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, DisplayName = "Current Health", Category = "Player Stats")
	float m_CurrentHealth{};

	UPROPERTY(EditAnywhere, DisplayName = "Max Shield", Category = "Player Stats")
	float m_MaxShield{100.f};

	UPROPERTY(ReplicatedUsing = OnRep_Shield, VisibleAnywhere, DisplayName = "Current Shield", Category = "Player Stats")
	float m_CurrentShield{0.f};

	UFUNCTION()
	void OnRep_Health(float lastHealth);

	UFUNCTION()
	void OnRep_Shield(float lastShieldValue);

	ABlasterPlayerController* m_pPlayerController{};
	bool m_IsAlive{};

	/*
	 * ELIMINATIONS
	 */
	FTimerHandle m_EliminationTimer{};
	void EliminationTimerFinished();

	UPROPERTY(EditDefaultsOnly, DisplayName = "Respawn Time", Category = "Player Stats")
	float m_RespawnTimer{5.f};

	UPROPERTY(VisibleAnywhere, DisplayName = "Dissolve Timeline Component", Category = "Animation|Dissolve")
	UTimelineComponent* m_pDissolveTimeLine{};
	FOnTimelineFloat m_DissolveTrack{};

	UFUNCTION()
	void UpdateDissolveMaterial(float dissolveValue);
	void StartDissolve();

	UPROPERTY(EditAnywhere, DisplayName = "Dissolve Curve", Category = "Animation|Dissolve")
	UCurveFloat* m_pDissolveCurve{};

	// The dynamic instance that we will update during run time
	UPROPERTY(VisibleAnywhere, DisplayName = "Dissolve Material", Category = "Animation|Dissolve")
	UMaterialInstanceDynamic* m_pDynamicDissolveMaterialInstance{};

	//The material instance set in the editor
	UPROPERTY(EditAnywhere, DisplayName = "Dissolve Material", Category = "Animation|Dissolve")
	UMaterialInstance* m_pDissolveMaterialInstance{};

	UPROPERTY(EditAnywhere, DisplayName = "Elimination Bot Effect", Category = "Animation|EliminationBot")
	UParticleSystem* m_pEliminationBotEffect{};

	UPROPERTY(EditAnywhere, DisplayName = "Elimination Bot Sound Effect", Category = "Animation|EliminationBot")
	USoundCue* m_pEliminationSound{};
	
	UParticleSystemComponent* m_pEliminationBotEffectComponent{};

	UPROPERTY(Replicated)
	bool m_DisabledGameplay{false};

	UPROPERTY(EditAnywhere, DisplayName = "Starting Weapon", Category = "Player Stats")
	TSubclassOf<AWeapon> m_DefaultWeaponClass;
	
private: // Functions
	UFUNCTION()
	void OnRep_OverlappingWeapon(const  AWeapon* const pOldWeapon) const;
	
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	//Some variables are not available for the first frames of the game in begin play so you should initialize them in this function
	void PollInitialize(float deltaTime);

	void MoveForward(const float value);
	void MoveRight(const float value);
	void Turn(const float value);
	void LookUp(const float value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void GrenadeThrowButtonPressed();
	void CalculateAimOffsetPitch();
	void CalculateAimOffset(float deltaTime);
	void SimulateProxiesTurn();
	void TurnInPlace(float deltaTime);
	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();

	UFUNCTION()
	void ReceiveDamage(AActor* damagedActor, float damage, const UDamageType* damageType, AController* instigatedBy, AActor* damageCauser);
	
public: // Getters & Setters
	void SetOverlappingWeapon(AWeapon* const pWeapon);
	bool IsWeaponEquipped() const;
	bool IsAiming() const;
	bool RotateRootBone() const { return m_RotateRootBone; }
	
	float GetAimOffsetYaw() const { return m_AimOffsetYaw; }
	float GetAimOffsetPitch() const { return m_AimOffsetPitch; }
	ETurningInPlace GetTurningInPlace() const { return m_TurningInPlace; }

	FVector GetHitTarget() const;

	AWeapon* GetEquippedWeapon() const;
	UCameraComponent* GetFollowCamera() const { return m_pFollowCamera; }
	UCombatComponent* GetCombatComponent() const { return m_pCombat; }
	UBuffComponent* GetBuffComponent() const { return m_pBuffComponent; }
	ULagCompensationComponent* GetLagCompensationComponent() const { return m_pLagCompensation; }
	
	bool IsAlive() const { return m_IsAlive; }
	bool IsLocallyReloading() const { return m_pCombat->m_IsLocallyReloading; }
	bool HasFinishedSwappingWeapons() const { return m_FinishedSwappingWeapons; }
	void SetFinishedSwappingWeapons(const bool finished) { m_FinishedSwappingWeapons = finished; }
	
	float GetMaxHealth() const { return m_MaxHealth; }
	float GetCurrentHealth() const { return m_CurrentHealth; }
	void AddHealth(const float amount) {m_CurrentHealth = FMath::Clamp(m_CurrentHealth + amount, 0.f, m_MaxHealth);}

	float GetMaxShield() const { return m_MaxShield; }
	float GetCurrentShield() const { return m_CurrentShield; }
	void AddShield(const float amount) { m_CurrentShield = FMath::Clamp(m_CurrentShield + amount, 0.f, m_MaxShield); }

	ECombatState GetCombatState() const;

	void SetDisabledGameplay(const bool disabled) { m_DisabledGameplay = disabled; }
	
};
