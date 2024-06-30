// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* pEliminatedPlayer,
                                        ABlasterPlayerController* pEliminatedPlayerController, ABlasterPlayerController* pAttackingPlayerController)
{
	checkf(pEliminatedPlayer, TEXT("PlayerEliminated: pEliminatedPlayer is nullptr"));

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

void ABlasterGameMode::BeginPlay()
{ 
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), m_PlayerStarts);
}
