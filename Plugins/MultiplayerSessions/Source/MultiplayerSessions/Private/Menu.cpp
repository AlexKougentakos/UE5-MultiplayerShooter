// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu.h"

#include "Components/Button.h"
#include "multiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 numberOfPublicConnections, FString matchType, FString lobbyPath)
{
	m_NumPublicConnections = numberOfPublicConnections;
	m_MatchType = matchType;
	m_LobbyPath = FString::Printf(TEXT("%s?listen"), *lobbyPath);
	
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	const UWorld* world = GetWorld();
	if (!world) return;

	APlayerController* playerController = world->GetFirstPlayerController();
	if (!playerController) return;

	FInputModeUIOnly inputMode;
	inputMode.SetWidgetToFocus(TakeWidget());
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	playerController->SetInputMode(inputMode);
	playerController->SetShowMouseCursor(true);

	const UGameInstance* gameInstance = GetGameInstance();
	if (!gameInstance) return;

	m_MultiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

	if (!m_MultiplayerSessionsSubsystem) return;

	m_MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
	m_MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
	m_MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	m_MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
	m_MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
	
}

bool UMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (!HostButton) return false;
	HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);

	if(!JoinButton) return false;
	JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	
	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	
	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool wasSuccessful)
{
	if (wasSuccessful)
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(
			TEXT("Session created!")));
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(
			TEXT("Session creation failed!")));
		return;
	}

	UWorld* world = GetWorld();
	if (!world) return;
	
	world->ServerTravel(m_LobbyPath);
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& sessionResults, bool wasSuccessful)
{
	if (!m_MultiplayerSessionsSubsystem) return;
	
	for (const auto result : sessionResults)
	{
		FString settingsValue{};
		result.Session.SessionSettings.Get(FName("MatchType"), settingsValue);

		if (settingsValue == m_MatchType)
		{
			m_MultiplayerSessionsSubsystem->JoinSession(result);
			
			//print the session we joined
			const FString sessionID = result.GetSessionIdStr();
			const FString user = result.Session.OwningUserName;
			if(GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan,
					FString::Printf(TEXT("Found session %s owned by %s"), *sessionID, *user));
			
			return;
		} 
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(
		TEXT("No session found!")));
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type result)
{
	 const auto sessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();

	if (!sessionInterface.IsValid()) return;

	FString address{};
	sessionInterface->GetResolvedConnectString(NAME_GameSession, address);

	const auto playerController = GetGameInstance()->GetFirstLocalPlayerController();
	playerController->ClientTravel(address, TRAVEL_Absolute);
}

void UMenu::OnDestroySession(bool wasSuccessful)
{
	
}

void UMenu::OnStartSession(bool wasSuccessful)
{
	
}

void UMenu::HostButtonClicked()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(
			TEXT("Host button clicked!")));

	if (!m_MultiplayerSessionsSubsystem) return;

	m_MultiplayerSessionsSubsystem->CreateSession(m_NumPublicConnections, m_MatchType);	
}

void UMenu::JoinButtonClicked()
{
	if (m_MultiplayerSessionsSubsystem)
		m_MultiplayerSessionsSubsystem->FindSessions(10000);
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();
	if(!world) return;

	APlayerController* playerController = world->GetFirstPlayerController();

	if (!playerController) return;
	
	playerController->SetInputMode(FInputModeGameOnly());
	playerController->SetShowMouseCursor(false);
	
}
