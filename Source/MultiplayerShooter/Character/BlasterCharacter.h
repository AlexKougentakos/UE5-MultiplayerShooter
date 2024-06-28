// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TurningInPlace.h"
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

public: //Variables
	UPROPERTY(EditAnywhere, Category = "Movement", DisplayName = "Player Rotation Angle Standing", meta = (ToolTip = "The angle at which the player rotates when turning in place when standing"))
	float m_StandingPlayerRotationAngle{80.f};

	UPROPERTY(EditAnywhere, Category = "Movement", DisplayName = "Player Rotation Angle Crouched", meta = (ToolTip = "The angle at which the player rotates when turning in place when crouched"))
	float m_CrouchingPlayerRotationAngle{60.f};

	UPROPERTY(EditAnywhere, Category = "Movement", DisplayName = "Player Rotation Speed")
	float m_RotationSpeed{5.f};

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

	UPROPERTY(EditAnywhere, DisplayName = "Fire Weapon Montage")
	UAnimMontage* m_pFireWeaponMontage{};
	
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
	void CalculateAimOffset(float deltaTime);
	void TurnInPlace(float deltaTime);
	void FireButtonPressed();
	void FireButtonReleased();
	
public: // Getters & Setters
	void SetOverlappingWeapon(AWeapon* const pWeapon);
	bool IsWeaponEquipped() const;
	bool IsAiming() const;
	
	float GetAimOffsetYaw() const { return m_AimOffsetYaw; }
	float GetAimOffsetPitch() const { return m_AimOffsetPitch; }
	AWeapon* GetEquippedWeapon() const;
	ETurningInPlace GetTurningInPlace() const { return m_TurningInPlace; }

	FVector GetHitTarget() const;

	UCameraComponent* GetFollowCamera() const { return m_pFollowCamera; }
};
