// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);

	const int32 numOfPlayers = (GameState.Get()->PlayerArray.Num());

	if (numOfPlayers == 2)
	{
		UWorld* world = GetWorld();
		checkf(world != nullptr, TEXT("World is null"));

		bUseSeamlessTravel = true;
		world->ServerTravel(FString("/Game/Levels/L_GameMap?listen"));
	}
}