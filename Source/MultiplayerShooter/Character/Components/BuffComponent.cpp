#include "BuffComponent.h"

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

