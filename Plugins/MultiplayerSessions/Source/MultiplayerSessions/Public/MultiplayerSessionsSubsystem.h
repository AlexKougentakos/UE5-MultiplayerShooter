// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"

//Declaring our own custom delegates to bind callbacks to
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, wasSuccsessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionComplete, const TArray<FOnlineSessionSearchResult>& sessionResults, bool wasSuccsessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, wasSuccsessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, wasSuccsessful);



/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMultiplayerSessionsSubsystem();

	//
	// To handle session functionality. To be called by the class that is responsible for the session
	//
	
	void CreateSession(int32 numPublicConnections, FString matchType);
	void FindSessions(int32 maxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& searchResult);
	void DestroySession();
	void StartSession();

	//
	// Our own custom delegates for the user class to bind callbacks to
	//
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionComplete MultiplayerOnFindSessionsComplete;
	FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;
	FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
	FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;
	
protected:
	
	//Internal callbacks for the delegates
	//These don't need to be called outside this class
	void OncCreateSessionComplete(FName sessionName, bool sessionCreated);
	void OnFindSessionsComplete(bool sessionFound);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void OnDestroySessionComplete(FName sessionName, bool sessionDestroyed);
	void OnStartSessionComplete(FName sessionName, bool sessionStarted);
	
private:
	IOnlineSessionPtr m_SessionInterface;
	TSharedPtr<FOnlineSessionSettings> m_LastSessionSettings{};
	TSharedPtr<FOnlineSessionSearch> m_LastSessionSearch{};

	//Delegate objects
	FOnCreateSessionCompleteDelegate m_OnCreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate m_OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate m_OnJoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate m_OnDestroySessionCompleteDelegate;
	FOnStartSessionCompleteDelegate m_OnStartSessionCompleteDelegate;

	//Delegate handles
	FDelegateHandle m_OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle m_OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle m_OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle m_OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle m_OnStartSessionCompleteDelegateHandle;

	bool m_CreateSessionOnDestroy{false};
	int32 m_LastNumPublicConnections{};
	FString m_LastMatchType{""};
};
