#include "BuffComponent.h"

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

void UBuffComponent::TickComponent(float deltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(deltaTime, TickType, ThisTickFunction);

	UpdateHealing(deltaTime);
	UpdateShield(deltaTime);
}

void UBuffComponent::Heal(const float amount, const float time)
{
	if (!m_pCharacter) return;
	m_IsHealing = true;

	m_HealingRate = amount / time;
	m_AmountToHeal += amount; //+= because we might pick up another one
}

void UBuffComponent::AddShield(const float amount, const float time)
{
	if (!m_pCharacter) return;
	m_IsApplyingShield = true;

	m_ShieldRate = amount / time;
	m_AmountToApplyShield += amount; //+= because we might pick up another one
}

void UBuffComponent::UpdateHealing(float deltaTime)
{
	if (!m_IsHealing || !m_pCharacter->IsAlive()) return;

	const float healAmountThisFrame = m_HealingRate * deltaTime;
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

void UBuffComponent::UpdateShield(float deltaTime)
{
	if (!m_IsApplyingShield || !m_pCharacter->IsAlive()) return;

	const float amountOfShieldThisFrame = m_ShieldRate * deltaTime;
	m_pCharacter->AddShield(amountOfShieldThisFrame);
	m_pCharacter->UpdateHudShield();
	m_AmountToApplyShield -= amountOfShieldThisFrame;

	if (m_AmountToApplyShield <= 0 ||
		m_pCharacter->GetCurrentShield() == m_pCharacter->GetMaxShield())
	{
		m_IsApplyingShield = false;
		m_AmountToApplyShield = 0;
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
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeed = m_InitialBaseSpeed;
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = m_InitialCrouchedSpeed;
	MulticastSpeedBuff(m_InitialBaseSpeed, m_InitialCrouchedSpeed);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float baseSpeed, float crouchSpeed)
{
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeed = baseSpeed;
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = crouchSpeed;
}

void UBuffComponent::BuffJump(const float jumpVelocity, const float time)
{
	m_pCharacter->GetWorldTimerManager().SetTimer(m_JumpBuffTimerHandle, this, &UBuffComponent::ResetJump, time, false);
	m_pCharacter->GetCharacterMovement()->JumpZVelocity = jumpVelocity;
	MulticastJumpBuff(jumpVelocity);
}

void UBuffComponent::MulticastJumpBuff_Implementation(float jumpVelocity)
{
	m_pCharacter->GetCharacterMovement()->JumpZVelocity = jumpVelocity;
}

void UBuffComponent::ResetJump()
{
	m_pCharacter->GetCharacterMovement()->JumpZVelocity = m_InitialJumpVelocity;
	MulticastJumpBuff(m_InitialJumpVelocity);
}


