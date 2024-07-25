#include "ReturnToMainMenu.h"

#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/GameModes/BlasterGameMode.h"
#include "MultiplayerShooter/GameModes/LobbyGameMode.h"

void UReturnToMainMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	const UWorld* pWorld = GetWorld();
	if (!pWorld) return;

	m_pPlayerController = pWorld->GetFirstPlayerController();
	if (!m_pPlayerController) return;

	FInputModeGameAndUI inputModeData{};
	inputModeData.SetWidgetToFocus(TakeWidget());
	m_pPlayerController->SetInputMode(inputModeData);
	m_pPlayerController->SetShowMouseCursor(true);

	const UGameInstance* pGameInstance = GetGameInstance();
	if (!pGameInstance) return;

	m_pSessionsSubsystem = pGameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (!m_pSessionsSubsystem) return;

	//if (!m_pSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
	m_pSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UReturnToMainMenu::OnDestroySession);

	checkf(ReturnButton, TEXT("ReturnButton is nullptr"));
	if (!ReturnButton->OnClicked.IsBound())
		ReturnButton->OnClicked.AddDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
}

void UReturnToMainMenu::MenuTeardown()
{
	RemoveFromParent();

	const UWorld* pWorld = GetWorld();
	if (!pWorld) return;

	APlayerController* pPlayerController = pWorld->GetFirstPlayerController();
	if (!pPlayerController) return;

	const FInputModeGameOnly inputModeData{};
	pPlayerController->SetInputMode(inputModeData);
	pPlayerController->SetShowMouseCursor(false);

	// Un-bind delegates
	if (ReturnButton->OnClicked.IsBound())
		ReturnButton->OnClicked.RemoveDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	if (m_pSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
		m_pSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UReturnToMainMenu::OnDestroySession);
}

bool UReturnToMainMenu::Initialize()
{
	if(!Super::Initialize()) return false;


	return true;
}

void UReturnToMainMenu::ReturnButtonClicked()
{
	ReturnButton->SetIsEnabled(false);
	const APlayerController* pPlayerController = GetWorld()->GetFirstPlayerController();
	if (!pPlayerController) return;

	const ALobbyGameMode* pGameMode = GetWorld()->GetAuthGameMode<ALobbyGameMode>();
	ABlasterCharacter* pCharacter = Cast<ABlasterCharacter>( pPlayerController->GetPawn());
	if (pGameMode && pCharacter) //we are in the waiting lobby, so we don't have any clean up to do so we can just leave
	{
		OnPlayerLeftGame();
		pCharacter->OnLeftGame.AddDynamic(this, &ThisClass::OnPlayerLeftGame);
		return;
	}
	
	if (pCharacter)
	{
		pCharacter->ServerLeaveGame();
		pCharacter->OnLeftGame.AddDynamic(this, &ThisClass::OnPlayerLeftGame);
	}
	else
	{
		ReturnButton->SetIsEnabled(true);
	}
}

void UReturnToMainMenu::OnDestroySession(const bool wasSuccessful)
{
	if (!wasSuccessful)
	{
		ReturnButton->SetIsEnabled(true);
		return;
	}
	
	const UWorld* pWorld = GetWorld();
	if (!pWorld) return;

	AGameModeBase* gameMode = pWorld->GetAuthGameMode<AGameModeBase>();
	if (gameMode)
	{
		gameMode->ReturnToMainMenuHost();
	}
	else
	{
		m_pPlayerController->ClientReturnToMainMenuWithTextReason(FText());
	}

	
}

void UReturnToMainMenu::OnPlayerLeftGame()
{
	m_pSessionsSubsystem->DestroySession();
}
