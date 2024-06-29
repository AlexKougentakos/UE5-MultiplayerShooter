// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TurningInPlace.h"
#include "Components/CombatComponent.h"
#include "MultiplayerShooter/Interfaces/InteractWithCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"

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

	void PlayFireMontage(const bool isAiming) const;
	void PlayHitReactMontage() const;
	
	UFUNCTION(NetMulticast, Unreliable) //Unreliable because it's a cosmetic effect that will mostly go unnoticed with so many hits
	void MulticastHit();

	virtual void OnRep_ReplicatedMovement() override;
	
private: // Variables
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* m_pCameraBoom{};

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* m_pFollowCamera{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* m_pOverheadWidget{};

	UPROPERTY(VisibleAnywhere)
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
	
};
