// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
class UButton;

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 numberOfPublicConnections = 4, FString matchType = FString(TEXT("FreeForAll")),
		FString lobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/L_Lobby")));

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;
	
	//
	// Callbacks for the delegates on the MultiplayerSessionSubsystem
	//
	UFUNCTION()
	void OnCreateSession(bool wasSuccessful);
	UFUNCTION()
	void OnDestroySession(bool wasSuccessful);
	UFUNCTION()
	void OnStartSession(bool wasSuccessful);
	
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& sessionResults, bool wasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type result);
private:
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();
	
	class UMultiplayerSessionsSubsystem* m_MultiplayerSessionsSubsystem{};

	int32 m_NumPublicConnections{4};
	FString m_MatchType{"FreeForAll"};
	FString m_LobbyPath{""};
};
