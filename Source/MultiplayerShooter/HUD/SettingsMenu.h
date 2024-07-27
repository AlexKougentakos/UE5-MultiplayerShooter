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
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta=(BindWidget))
	UButton* BackButton{};

	UPROPERTY(meta=(BindWidget))
	UButton* ApplyButton{};

	UPROPERTY(meta=(BindWidget))
	USlider* ADS_MouseSensitivitySlider{};
	
	UPROPERTY(meta=(BindWidget))
	USlider* MouseSensitivitySlider{};

private:
	UFUNCTION()
	void OnADSSensitivityChanged(float value);
	
	UFUNCTION()
	void OnMouseSensitivityChanged(float value);

	float m_TemporaryMouseSensitivity{};
	float m_TemporaryADSSensitivity{};
	
	UFUNCTION()
	void ApplySensitiveSettings();
};
