// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TurningInPlace.h"
#include "Components/CombatComponent.h"
#include "Components/TimelineComponent.h"
#include "MultiplayerShooter/Interfaces/InteractWithCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"

class USoundCue;
class UCombatComponent;
class AWeapon;
class UWidgetComponent;
//Forward declarations
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
	
	UFUNCTION(NetMulticast, Unreliable) //Unreliable because it's a cosmetic effect that will mostly go unnoticed with so many hits
	void MulticastHit();

	virtual void OnRep_ReplicatedMovement() override;

private: // Variables
	UPROPERTY(VisibleAnywhere, Category = Camera, DisplayName = "Camera Boom")
	USpringArmComponent* m_pCameraBoom{};

	UPROPERTY(VisibleAnywhere, Category = Camera, DisplayName = "Follow Camera")
	UCameraComponent* m_pFollowCamera{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), DisplayName = "Overhead Widget")
	UWidgetComponent* m_pOverheadWidget{};

	UPROPERTY(VisibleAnywhere, DisplayName = "Combat Component")
	UCombatComponent* m_pCombat{};

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* m_pOverlappingWeapon{};

	float m_AimOffsetPitch{};
	float m_AimOffsetYaw{};
	float m_InterpolatedAimOffsetYaw{};

	FRotator m_LastFrameRotation{};

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
	 * Player Health
	 */

	UPROPERTY(EditAnywhere, DisplayName = "Max Health", Category = "Player Stats")
	float m_MaxHealth{100.f};

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, DisplayName = "Current Health", Category = "Player Stats")
	float m_CurrentHealth{};

	UFUNCTION()
	void OnRep_Health();

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

	
	
private: // Functions
	UFUNCTION()
	void OnRep_OverlappingWeapon(const  AWeapon* const pOldWeapon) const;
	
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();


	void MoveForward(const float value);
	void MoveRight(const float value);
	void Turn(const float value);
	void LookUp(const float value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void CalculateAimOffsetPitch();
	void CalculateAimOffset(float deltaTime);
	void SimulateProxiesTurn();
	void TurnInPlace(float deltaTime);
	void FireButtonPressed();
	void FireButtonReleased();

	UFUNCTION()
	void ReceiveDamage(AActor* damagedActor, float damage, const UDamageType* damageType, AController* instigatedBy, AActor* damageCauser);
	
public: // Getters & Setters
	void SetOverlappingWeapon(AWeapon* const pWeapon);
	bool IsWeaponEquipped() const;
	bool IsAiming() const;
	bool RotateRootBone() const { return m_RotateRootBone; }
	
	float GetAimOffsetYaw() const { return m_AimOffsetYaw; }
	float GetAimOffsetPitch() const { return m_AimOffsetPitch; }
	AWeapon* GetEquippedWeapon() const;
	ETurningInPlace GetTurningInPlace() const { return m_TurningInPlace; }

	FVector GetHitTarget() const;

	UCameraComponent* GetFollowCamera() const { return m_pFollowCamera; }
	bool IsAlive() const { return m_IsAlive; }

	float GetMaxHealth() const { return m_MaxHealth; }
	float GetCurrentHealth() const { return m_CurrentHealth; }
};
