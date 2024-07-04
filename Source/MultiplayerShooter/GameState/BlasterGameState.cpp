// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameState.h"

#include "MultiplayerShooter/PlayerState/BlasterPlayerState.h"
#include "Net/UnrealNetwork.h"

void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterGameState, m_TopScoringPlayers);
}

void ABlasterGameState::UpdateTopScore(ABlasterPlayerState* pScoringPlayer)
{
	if (m_TopScoringPlayers.IsEmpty())
	{
		m_TopScoringPlayers.Emplace(pScoringPlayer);
		m_TopScore = pScoringPlayer->GetScore();
	}
	else if (pScoringPlayer->GetScore() == m_TopScore)
	{
		m_TopScoringPlayers.AddUnique(pScoringPlayer);
	}
	else if (pScoringPlayer->GetScore() > m_TopScore)
	{
		m_TopScoringPlayers.Empty();
		m_TopScoringPlayers.Emplace(pScoringPlayer);
		m_TopScore = pScoringPlayer->GetScore();
	}
}
