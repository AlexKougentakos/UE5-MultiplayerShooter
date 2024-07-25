#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyController.generated.h"

class UReturnToMainMenu;

UCLASS()
class MULTIPLAYERSHOOTER_API ALobbyController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "UI", DisplayName = "Return To Main Menu Widget Class")
	TSubclassOf<UUserWidget> m_ReturnToMainMenuWidgetClass{};
	UPROPERTY() UReturnToMainMenu* m_pReturnToMainMenuWidget{};
	bool m_IsReturnToMainMenuOpen{false};
	void ShowReturnToMainMenu();
};
