// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimationInstance.h"

#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/Weapon/Weapon.h"

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

	UpdateVariables();

	HandleLeaning(deltaTime);

	m_AimOffsetYaw = m_pBlasterCharacter->GetAimOffsetYaw();
	m_AimOffsetPitch = m_pBlasterCharacter->GetAimOffsetPitch();

	if (m_IsWeaponEquipped)
	{
		ApplyInverseKinematicsToHand(deltaTime);
	}
}

void UCharacterAnimationInstance::UpdateVariables()
{
	FVector velocity = m_pBlasterCharacter->GetVelocity();
	velocity.Z = 0.f;
	m_Speed = velocity.Size();

	m_IsInAir = m_pBlasterCharacter->GetCharacterMovement()->IsFalling();
	m_IsAccelerating = m_pBlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	m_IsWeaponEquipped = m_pBlasterCharacter->IsWeaponEquipped();
	m_pEquippedWeapon = m_pBlasterCharacter->GetEquippedWeapon();
	m_IsCrouched = m_pBlasterCharacter->bIsCrouched;
	m_IsAiming = m_pBlasterCharacter->IsAiming();
	m_TurningInPlace = m_pBlasterCharacter->GetTurningInPlace();
	m_RotateRootBone = m_pBlasterCharacter->RotateRootBone();
	m_IsAlive = m_pBlasterCharacter->IsAlive();

	switch (m_pBlasterCharacter->GetCombatState())
	{
	case ECombatState::ECS_Unoccupied:
		m_UseFABRIK = true;
		break;
	case ECombatState::ECS_Reloading:
		m_UseFABRIK = false;
		break;
	case ECombatState::ECS_MAX:
		m_UseFABRIK = true;
		break;
	}
}

void UCharacterAnimationInstance::HandleLeaning(float deltaTime)
{
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
}

void UCharacterAnimationInstance::ApplyInverseKinematicsToHand(const float deltaTime)
{
	const auto pWeaponMesh = m_pEquippedWeapon->GetWeaponMesh();
	const auto pCharacterMesh = m_pBlasterCharacter->GetMesh();
		
	checkf(m_pEquippedWeapon, TEXT("Equipped weapon is nullptr while equipped is true"));
	checkf(pWeaponMesh, TEXT("Equipped weapon mesh is nullptr"));
	checkf(pCharacterMesh, TEXT("Character mesh is nullptr"));

	m_LeftHandTransform = pWeaponMesh->GetSocketTransform(FName("LeftHandSocket"), RTS_World);

	FVector outPosition{};
	FRotator outRotation{};
	pCharacterMesh->TransformToBoneSpace(FName("hand_r"), m_LeftHandTransform.GetLocation(), FRotator::ZeroRotator, outPosition, outRotation);
	m_LeftHandTransform.SetLocation(outPosition);
	m_LeftHandTransform.SetRotation(FQuat(outRotation));

	if (!m_pBlasterCharacter->IsLocallyControlled()) return;
	m_IsLocallyControlled = true;
	const FTransform rightHandTransform = m_pBlasterCharacter->GetMesh()->GetSocketTransform(FName("hand_r"), RTS_World);
	const FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(FVector3d(), rightHandTransform.GetLocation() - m_pBlasterCharacter->GetHitTarget());
	m_RightHandRotation = FMath::RInterpTo(m_RightHandRotation, lookAtRotation, deltaTime, 30.f);

	m_UseFABRIK = m_pBlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	m_UseAimOffsets = m_pBlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
	m_TransformRightHand = m_pBlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
}