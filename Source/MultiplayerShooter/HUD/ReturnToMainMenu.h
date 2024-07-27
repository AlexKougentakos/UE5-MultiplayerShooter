#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReturnToMainMenu.generated.h"

class USettingsMenu;
class UCanvasPanel;
class UMultiplayerSessionsSubsystem;
class UButton;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API UReturnToMainMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	void MenuSetup();
	void MenuTeardown();

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta=(BindWidget))
	UButton* ReturnButton{};
	
	UPROPERTY(meta=(BindWidget))
	UButton* OptionsButton{};

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* PauseScreen{};

	UPROPERTY(meta=(BindWidget))
	USettingsMenu* SettingsMenu{};

	UFUNCTION()
	void ReturnButtonClicked();

	UFUNCTION()
	void OptionsButtonClicked();

	UFUNCTION()
	void GoBackToPauseScreen();

	UPROPERTY() UMultiplayerSessionsSubsystem* m_pSessionsSubsystem{};

	UPROPERTY() APlayerController* m_pPlayerController{};
	UFUNCTION() void OnDestroySession(const bool wasSuccessful);

	UFUNCTION()
	void OnPlayerLeftGame();
};
