#include "SettingsMenu.h"

#include "Components/Slider.h"
#include "MultiplayerShooter/Saving/SensitivitySettings.h"


void USettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	ADS_MouseSensitivitySlider->SetValue(USensitivitySettings::LoadSensitivitySettings()->GetADSSensitivityMultiplier());
	MouseSensitivitySlider->SetValue(USensitivitySettings::LoadSensitivitySettings()->GetMouseSensitivityMultiplier());

	ADS_MouseSensitivitySlider->OnValueChanged.AddDynamic(this, &USettingsMenu::OnADSSensitivityChanged);
	MouseSensitivitySlider->OnValueChanged.AddDynamic(this, &USettingsMenu::OnMouseSensitivityChanged);
}

void USettingsMenu::ApplySensitiveSettings(const float MouseSensitivity, const float ADSSensitivity) const
{
	USensitivitySettings* pSettings = USensitivitySettings::LoadSensitivitySettings();
	pSettings->SetMouseSensitivity(MouseSensitivity);
	pSettings->SetADSSensitivity(ADSSensitivity);
	pSettings->SaveSensitivitySettings();
}

void USettingsMenu::OnADSSensitivityChanged(float value)
{
	ApplySensitiveSettings(MouseSensitivitySlider->GetValue(), value);
}

void USettingsMenu::OnMouseSensitivityChanged(float value)
{
	ApplySensitiveSettings(value, ADS_MouseSensitivitySlider->GetValue());
}
