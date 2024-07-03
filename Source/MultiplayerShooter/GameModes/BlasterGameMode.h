// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "BlasterGameMode.generated.h"

namespace MatchState
{
	extern MULTIPLAYERSHOOTER_API const FName Cooldown; // Match has been finished, display winner and begin countdown timer
}

class ABlasterPlayerController;
class ABlasterCharacter;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ABlasterGameMode();
	virtual void PlayerEliminated(ABlasterCharacter* pEliminatedPlayer, ABlasterPlayerController* pEliminatedPlayerController, ABlasterPlayerController*
	                              pAttackingPlayerController);

	virtual void RequestRespawn(ABlasterCharacter* pCharacterToRespawn, ABlasterPlayerController* pPlayerController);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnMatchStateSet() override;
private:
	TArray<AActor*> m_PlayerStarts{};

	UPROPERTY(EditAnywhere, Category = "GameMode", DisplayName = "Warm Up Duration")
	float m_WarmUpDuration{10.f};
	UPROPERTY(EditAnywhere, Category = "GameMode", DisplayName = "Game Duration")
	float m_GameDuration{120.f};
	UPROPERTY(EditAnywhere, Category = "GameMode", DisplayName = "Cooldown Duration")
	float m_CooldownDuration{10.f};
	
	float m_CountdownTime{0.f};

	float m_LevelStartingTime{0.f}; // The time at which we loaded into the map

public:
	float GetGameDuration() const {return m_GameDuration; }
	float GetWarmUpDuration() const {return m_WarmUpDuration; }
	float GetLevelStartingTime() const {return m_LevelStartingTime; }
};
