#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenu.generated.h"

class USlider;
class UButton;

UCLASS()
class MULTIPLAYERSHOOTER_API USettingsMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	UButton* BackButton{};

	UPROPERTY(meta=(BindWidget))
	USlider* ADS_MouseSensitivitySlider{};
	
	UPROPERTY(meta=(BindWidget))
	USlider* MouseSensitivitySlider{};

	void ApplySensitiveSettings(const float MouseSensitivity, const float ADSSensitivity) const;
private:
	UFUNCTION()
	void OnADSSensitivityChanged(float value);
	
	UFUNCTION()
	void OnMouseSensitivityChanged(float value);
};
