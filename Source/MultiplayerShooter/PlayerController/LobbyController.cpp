#include "LobbyController.h"

#include "Blueprint/UserWidget.h"
#include "MultiplayerShooter/HUD/ReturnToMainMenu.h"

void ALobbyController::SetupInputComponent()
{
	Super::SetupInputComponent();

	checkf(InputComponent, TEXT("InputComponent is null"));
	InputComponent->BindAction("Quit", IE_Pressed, this, &ThisClass::ShowReturnToMainMenu);
}

void ALobbyController::ShowReturnToMainMenu()
{
	checkf(m_ReturnToMainMenuWidgetClass, TEXT("Return to main menu widget class is null"));
	
	if (!m_pReturnToMainMenuWidget)
	{
		m_pReturnToMainMenuWidget = CreateWidget<UReturnToMainMenu>(this, m_ReturnToMainMenuWidgetClass);
	}

	m_IsReturnToMainMenuOpen = !m_IsReturnToMainMenuOpen;
	if (m_IsReturnToMainMenuOpen)
	{
		m_pReturnToMainMenuWidget->MenuSetup();
	}
	else
	{
		m_pReturnToMainMenuWidget->MenuTeardown();
	}
}