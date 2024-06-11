// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimationInstance.h"

#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCharacterAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	m_pBlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UCharacterAnimationInstance::NativeUpdateAnimation(float deltaTime)
{
	Super::NativeUpdateAnimation(deltaTime);

	if (!m_pBlasterCharacter)
	{
		m_pBlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}

	if (!m_pBlasterCharacter) return;

	FVector velocity = m_pBlasterCharacter->GetVelocity();
	velocity.Z = 0.f;
	m_Speed = velocity.Size();

	m_IsInAir = m_pBlasterCharacter->GetCharacterMovement()->IsFalling();
	m_IsAccelerating = m_pBlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	m_IsWeaponEquipped = m_pBlasterCharacter->IsWeaponEquipped();
	m_IsCrouched = m_pBlasterCharacter->bIsCrouched;
	m_IsAiming = m_pBlasterCharacter->IsAiming();

	//Offset the Yaw for Strafing left and right while aiming in the same direction,
	//you don't want the weapon to be pointing in the direction of the movement
	const FRotator aimRotation =  m_pBlasterCharacter->GetBaseAimRotation();
	const FRotator movementRotation = UKismetMathLibrary::MakeRotFromX(m_pBlasterCharacter->GetVelocity());
	const FRotator deltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation);
	m_DeltaRotation = FMath::RInterpTo(m_DeltaRotation, deltaRotation, deltaTime, 5.f);
	m_YawOffset = m_DeltaRotation.Yaw;

	m_LastFrameRotation = m_CurrentRotation;
	m_CurrentRotation = m_pBlasterCharacter->GetActorRotation();
	const FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(m_CurrentRotation, m_LastFrameRotation);
	const float target = delta.Yaw / deltaTime;
	const float interpolated = FMath::FInterpTo(m_Lean, target, deltaTime, 6.f);
	m_Lean = FMath::Clamp(interpolated, -45.f, 45.f);

	m_AimOffsetYaw = m_pBlasterCharacter->GetAimOffsetYaw();
	m_AimOffsetPitch = m_pBlasterCharacter->GetAimOffsetPitch();
}
