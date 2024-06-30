// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "BlasterGameMode.generated.h"

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
	virtual void PlayerEliminated(ABlasterCharacter* pEliminatedPlayer, ABlasterPlayerController* pEliminatedPlayerController, ABlasterPlayerController*
	                              pAttackingPlayerController);

	virtual void RequestRespawn(ABlasterCharacter* pCharacterToRespawn, ABlasterPlayerController* pPlayerController);

	virtual void BeginPlay() override;
private:
	TArray<AActor*> m_PlayerStarts{};
};