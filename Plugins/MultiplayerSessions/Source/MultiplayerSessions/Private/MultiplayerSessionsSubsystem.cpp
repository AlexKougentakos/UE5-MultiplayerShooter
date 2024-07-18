// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	m_OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(
		this, &ThisClass::OncCreateSessionComplete)),
	m_OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(
		this, &ThisClass::OnFindSessionsComplete)),
	m_OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(
		this, &ThisClass::OnJoinSessionComplete)),
	m_OnDestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(
		this, &ThisClass::OnDestroySessionComplete)),
	m_OnStartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(
		this, &ThisClass::OnStartSessionComplete))
{
	const IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();

	if (!subsystem)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,  FString::Printf(TEXT("No online subsystem found!")));
		
		return;
	}

	m_SessionInterface = subsystem->GetSessionInterface();

	
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 numPublicConnections, FString matchType)
{
	if (!m_SessionInterface.IsValid()) return;

	const auto existingSession = m_SessionInterface->GetNamedSession(NAME_GameSession);
	if (existingSession)
	{
		m_CreateSessionOnDestroy = true;
		m_LastNumPublicConnections = numPublicConnections;
		m_LastMatchType = matchType;
		DestroySession();
	}

	//Store the delegate in an FDelegateHandle so we can remove it later
	m_OnCreateSessionCompleteDelegateHandle = m_SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(m_OnCreateSessionCompleteDelegate);

	m_LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	m_LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	m_LastSessionSettings->NumPublicConnections = numPublicConnections;
	m_LastSessionSettings->bAllowJoinInProgress = true;
	m_LastSessionSettings->bAllowJoinViaPresence = true;
	m_LastSessionSettings->bUseLobbiesIfAvailable = true;
	m_LastSessionSettings->bShouldAdvertise = true;
	m_LastSessionSettings->bUsesPresence = true;
	m_LastSessionSettings->Set(FName("MatchType"), matchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	m_LastSessionSettings->BuildUniqueId = 1;

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!m_SessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *m_LastSessionSettings))
	{
		m_SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(m_OnCreateSessionCompleteDelegateHandle);

		//Broadcast our own custom delegate
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 maxSearchResults)
{
	if (!m_SessionInterface.IsValid()) return;

	m_OnFindSessionsCompleteDelegateHandle = m_SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(m_OnFindSessionsCompleteDelegate);

	m_LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	m_LastSessionSearch->MaxSearchResults = maxSearchResults;
	m_LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	
	m_LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!m_SessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), m_LastSessionSearch.ToSharedRef()))
	{
		m_SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(m_OnFindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>{}, false);
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan,
		FString::Printf(TEXT("Found %d sessions"), m_LastSessionSearch->SearchResults.Num()));
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& searchResult)
{
	if (!m_SessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	
	m_OnJoinSessionCompleteDelegateHandle =	m_SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(m_OnJoinSessionCompleteDelegate);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (m_SessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, searchResult))
	{
		m_SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(m_OnJoinSessionCompleteDelegateHandle);

		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!m_SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	m_OnDestroySessionCompleteDelegateHandle =
		m_SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(m_OnDestroySessionCompleteDelegate);
	
	if (!m_SessionInterface->DestroySession(NAME_GameSession))
	{
		m_SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(m_OnDestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
	
}

void UMultiplayerSessionsSubsystem::OncCreateSessionComplete(FName sessionName, bool sessionCreated)
{
	if (!m_SessionInterface) return;

	m_SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(m_OnCreateSessionCompleteDelegateHandle);
	
	MultiplayerOnCreateSessionComplete.Broadcast(sessionCreated);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool sessionFound)
{
	if (!m_SessionInterface) return;

	m_SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(m_OnFindSessionsCompleteDelegateHandle);
	
	if (m_LastSessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>{}, false);
		return;
	}
	
	MultiplayerOnFindSessionsComplete.Broadcast(m_LastSessionSearch->SearchResults, sessionFound);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (!m_SessionInterface.IsValid()) return;
	
	m_SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(m_OnJoinSessionCompleteDelegateHandle);

	MultiplayerOnJoinSessionComplete.Broadcast(result);
	
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName sessionName, bool sessionDestroyed)
{
	if (m_SessionInterface)
	{
		m_SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(m_OnDestroySessionCompleteDelegateHandle);
	}
	if (sessionDestroyed && m_CreateSessionOnDestroy)
	{
		m_CreateSessionOnDestroy = false;
		CreateSession(m_LastNumPublicConnections, m_LastMatchType);
	}
	MultiplayerOnDestroySessionComplete.Broadcast(sessionDestroyed);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName sessionName, bool sessionStarted)
{
}
