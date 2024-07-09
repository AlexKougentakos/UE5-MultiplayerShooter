﻿#include "BuffComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"

UBuffComponent::UBuffComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
}


void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateHealing(DeltaTime);
}

void UBuffComponent::Heal(const float amount, const float time)
{
	if (!m_pCharacter) return;
	m_IsHealing = true;

	m_HealingRate = amount / time;
	m_AmountToHeal += amount; //+= because we might pick up another one
}

void UBuffComponent::UpdateHealing(float DeltaTime)
{
	if (!m_IsHealing || !m_pCharacter->IsAlive()) return;

	const float healAmountThisFrame = m_HealingRate * DeltaTime;
	m_pCharacter->AddHealth(healAmountThisFrame);
	m_pCharacter->UpdateHudHealth();
	m_AmountToHeal -= healAmountThisFrame;

	if (m_AmountToHeal <= 0 ||
		m_pCharacter->GetCurrentHealth() == m_pCharacter->GetMaxHealth())
	{
		m_IsHealing = false;
		m_AmountToHeal = 0;
	}
}

void UBuffComponent::BuffSpeed(const float baseSpeed, const float crouchedSpeed, const float time)
{
	if (!m_pCharacter) return;

	m_pCharacter->GetWorldTimerManager().SetTimer(m_SpeedBuffTimerHandle, this, &UBuffComponent::ResetSpeed, time, false);
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeed = baseSpeed;
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = crouchedSpeed;

	MulticastSpeedBuff(baseSpeed, crouchedSpeed);
}

void UBuffComponent::ResetSpeed()
{
	UE_LOG(LogTemp, Warning, TEXT("Resetting speed"));
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeed = m_InitialBaseSpeed;
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = m_InitialCrouchedSpeed;
	MulticastSpeedBuff(m_InitialBaseSpeed, m_InitialCrouchedSpeed);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float baseSpeed, float crouchSpeed)
{
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeed = baseSpeed;
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = crouchSpeed;
}

