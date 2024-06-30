#include "BlasterPlayerState.h"

#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"


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
