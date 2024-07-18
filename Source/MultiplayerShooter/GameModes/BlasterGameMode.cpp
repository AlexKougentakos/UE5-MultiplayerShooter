// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/GameState/BlasterGameState.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"
#include "MultiplayerShooter/PlayerState/BlasterPlayerState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;	
}

void ABlasterGameMode::BeginPlay()
{ 
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), m_PlayerStarts);

	m_LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MatchState == MatchState::WaitingToStart)
	{
		m_CountdownTime = m_WarmUpDuration - GetWorld()->GetTimeSeconds() + m_LevelStartingTime;
		if (m_CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		m_CountdownTime = m_WarmUpDuration +  m_GameDuration - GetWorld()->GetTimeSeconds() + m_LevelStartingTime;
		if (m_CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		m_CountdownTime = m_CooldownDuration + m_WarmUpDuration +  m_GameDuration - GetWorld()->GetTimeSeconds() + m_LevelStartingTime;
		if (m_CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	// Let all the players know the match state has changed
	for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
	{
		ABlasterPlayerController* pPlayerController = Cast<ABlasterPlayerController>(*it);
		if (pPlayerController)
		{
			pPlayerController->OnMatchStateSet(MatchState);
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* pEliminatedPlayer,
                                        ABlasterPlayerController* pEliminatedPlayerController, ABlasterPlayerController* pAttackingPlayerController)
{
	checkf(pEliminatedPlayer, TEXT("PlayerEliminated: pEliminatedPlayer is nullptr"));
	ABlasterPlayerState* pEliminatedPlayerState = pEliminatedPlayerController ? pEliminatedPlayerController->GetPlayerState<ABlasterPlayerState>() : nullptr;
	ABlasterPlayerState* pAttackingPlayerState = pAttackingPlayerController ? pAttackingPlayerController->GetPlayerState<ABlasterPlayerState>() : nullptr;

	const auto pGameState = GetGameState<ABlasterGameState>();
	checkf(pGameState, TEXT("PlayerEliminated: pGameState is nullptr"));
	
	if (pAttackingPlayerState && pAttackingPlayerState != pEliminatedPlayerState)
	{
		//Get the top scoring players BEFORE updating the positions
		TArray<ABlasterPlayerState*> topScoringPlayers = pGameState->GetTopScoringPlayers();
		
		pAttackingPlayerState->AddToSore(1.0f);
		pGameState->UpdateTopScore(pAttackingPlayerState);

		for (int i{}; i < topScoringPlayers.Num(); ++i)
		{
			//If the CURRENT top scoring player array DOES NOT contain one of the old players
			//that means that he lost the lead
			if (!pGameState->GetTopScoringPlayers().Contains(topScoringPlayers[i]))
			{
				 Cast<ABlasterCharacter>(topScoringPlayers[i]->GetPawn())->MulticastLostTheLead();
			}
		}

		// Check if the current top scorer is a new top scorer
		for (const auto& currentTopScorer : pGameState->GetTopScoringPlayers())
		{
			bool isNewTopScorer{true};
			for (const auto& oldTopScorer : topScoringPlayers)
			{
				//If any of the old top scorers is the same as the current top scorer
				//then they are not a new top scorer
				if (oldTopScorer == currentTopScorer) isNewTopScorer = false;
			}

			if (isNewTopScorer)
				Cast<ABlasterCharacter>(currentTopScorer->GetPawn())->MulticastGainedTheLead();
		}
	}
	pEliminatedPlayerState->AddToDeaths();
		
	//No need to check for authority here, we are in the game mode which only exists on the server
	pEliminatedPlayer->Eliminated();
}

void ABlasterGameMode::RequestRespawn(ABlasterCharacter* pCharacterToRespawn,
	ABlasterPlayerController* pPlayerController)
{
	checkf(pCharacterToRespawn, TEXT("RequestRespawn: pCharacterToRespawn is nullptr"));
	checkf(pPlayerController, TEXT("RequestRespawn: pPlayerController is nullptr"));

	pCharacterToRespawn->Reset();
	pCharacterToRespawn->Destroy();
	
	const int32 selection = FMath::RandRange(0, m_PlayerStarts.Num() - 1);
	RestartPlayerAtPlayerStart(pPlayerController, m_PlayerStarts[selection]);
}


void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* pPlayerState)
{
	//we have to remove the player from the top scoring players list
	//otherwise the reference is null
	ABlasterGameState* pGameState = GetGameState<ABlasterGameState>();
	if (pGameState && pGameState->GetTopScoringPlayers().Contains(pPlayerState))
	{
		pGameState->GetTopScoringPlayers().Remove(pPlayerState);
	}

	ABlasterCharacter* pCharacter =  Cast<ABlasterCharacter>(pPlayerState->GetPawn());
	if (pCharacter) pCharacter->Eliminated(true);
}
