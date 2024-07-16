#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReturnToMainMenu.generated.h"

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
	UButton* ReturnButton;

	UFUNCTION()
	void ReturnButtonClicked();

	UMultiplayerSessionsSubsystem* m_pSessionsSubsystem{};

	APlayerController* m_pPlayerController{};
	UFUNCTION()
	void OnDestroySession(const bool wasSuccessful);
};
