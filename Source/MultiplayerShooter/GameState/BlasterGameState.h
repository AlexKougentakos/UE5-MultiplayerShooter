// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

class ABlasterPlayerState;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

	void UpdateTopScore(ABlasterPlayerState* pScoringPlayer);
	
private:
	UPROPERTY(Replicated)
   	TArray<ABlasterPlayerState*> m_TopScoringPlayers{};

	float m_TopScore{};	
public:
	const TArray<ABlasterPlayerState*>& GetTopScoringPlayers() const {return m_TopScoringPlayers;}
};
