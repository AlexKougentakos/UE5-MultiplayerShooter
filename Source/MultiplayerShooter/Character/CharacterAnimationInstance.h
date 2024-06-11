// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimationInstance.generated.h"

class ABlasterCharacter;
UCLASS()
class MULTIPLAYERSHOOTER_API UCharacterAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float deltaTime) override;
	
private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"), DisplayName = "BlasterCharacter")
	ABlasterCharacter* m_pBlasterCharacter{};

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"), DisplayName = "Speed")
	float m_Speed{};

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"), DisplayName = "IsInAir")
	bool m_IsInAir{};

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"), DisplayName = "IsAccelerating")
	bool m_IsAccelerating{};

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"), DisplayName = "IsWeaponEquipped")
	bool m_IsWeaponEquipped{};

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"), DisplayName = "IsCrouched")
	bool m_IsCrouched{};

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"), DisplayName = "IsAiming")
	bool m_IsAiming{};
	
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"), DisplayName = "Yaw Offset")
	float m_YawOffset{};
	
	 UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"), DisplayName = "Lean")
	 float m_Lean{};

	UPROPERTY(BlueprintReadOnly, Category = Aiming, meta = (AllowPrivateAccess = "true"), DisplayName = "Aim Offset Yaw")
	float m_AimOffsetYaw{};
	
	UPROPERTY(BlueprintReadOnly, Category = Aiming, meta = (AllowPrivateAccess = "true"), DisplayName = "Aim Offset Pitch")
	float m_AimOffsetPitch{};
	
	FRotator m_LastFrameRotation{};
	FRotator m_CurrentRotation{};
	FRotator m_DeltaRotation{};
};
