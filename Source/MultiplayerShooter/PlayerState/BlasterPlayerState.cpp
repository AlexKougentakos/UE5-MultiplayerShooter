#include "BlasterPlayerState.h"

#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, m_Deaths);
}

void ABlasterPlayerState::AddToSore(float scoreAmount)
{
	SetScore(GetScore() + scoreAmount);
	
	m_pBlasterCharacter = m_pBlasterCharacter ? m_pBlasterCharacter : Cast<ABlasterCharacter>(GetPawn());
	if (m_pBlasterCharacter)
	{
		m_pBlasterPlayerController = m_pBlasterPlayerController ? m_pBlasterPlayerController : Cast<ABlasterPlayerController>(m_pBlasterCharacter->GetController());
		if (m_pBlasterPlayerController)
		{
			m_pBlasterPlayerController->SetHudScore(GetScore());
		}
	}
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	
	m_pBlasterCharacter = m_pBlasterCharacter ? m_pBlasterCharacter : Cast<ABlasterCharacter>(GetPawn());
	if (m_pBlasterCharacter)
	{
		m_pBlasterPlayerController = m_pBlasterPlayerController ? m_pBlasterPlayerController : Cast<ABlasterPlayerController>(m_pBlasterCharacter->GetController());
		if (m_pBlasterPlayerController)
		{
			m_pBlasterPlayerController->SetHudScore(GetScore());
		}
	}
		
}


void ABlasterPlayerState::AddToDeaths(int deathsAmount)
{
	m_Deaths += deathsAmount;
	
	m_pBlasterCharacter = m_pBlasterCharacter ? m_pBlasterCharacter : Cast<ABlasterCharacter>(GetPawn());
	if (m_pBlasterCharacter)
	{
		m_pBlasterPlayerController = m_pBlasterPlayerController ? m_pBlasterPlayerController : Cast<ABlasterPlayerController>(m_pBlasterCharacter->GetController());
		if (m_pBlasterPlayerController)
		{
			m_pBlasterPlayerController->SetHudDeaths(m_Deaths);
		}
	}
}

void ABlasterPlayerState::OnRep_Deaths()
{
	m_pBlasterCharacter = m_pBlasterCharacter ? m_pBlasterCharacter : Cast<ABlasterCharacter>(GetPawn());
	if (m_pBlasterCharacter)
	{
		m_pBlasterPlayerController = m_pBlasterPlayerController ? m_pBlasterPlayerController : Cast<ABlasterPlayerController>(m_pBlasterCharacter->GetController());
		if (m_pBlasterPlayerController)
		{
			m_pBlasterPlayerController->SetHudDeaths(m_Deaths);
		}
	}
}
