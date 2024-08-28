#include "SettingsMenu.h"

#include "Components/Button.h"
#include "Components/Slider.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Saving/SensitivitySettings.h"


void USettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Load sensitivity settings
	const auto SensitivitySettings = USensitivitySettings::LoadSensitivitySettings();
	m_TemporaryADSSensitivity = SensitivitySettings->GetADSSensitivityMultiplier();
	m_TemporaryMouseSensitivity = SensitivitySettings->GetMouseSensitivityMultiplier();
	
	ADS_MouseSensitivitySlider->SetValue(m_TemporaryADSSensitivity);
	MouseSensitivitySlider->SetValue(m_TemporaryMouseSensitivity);

	if (!ApplyButton->OnClicked.IsBound())
		ApplyButton->OnClicked.AddDynamic(this, &USettingsMenu::ApplySensitiveSettings);
	if (!ADS_MouseSensitivitySlider->OnValueChanged.IsBound())
		ADS_MouseSensitivitySlider->OnValueChanged.AddDynamic(this, &USettingsMenu::OnADSSensitivityChanged);
	if (!MouseSensitivitySlider->OnValueChanged.IsBound())
		MouseSensitivitySlider->OnValueChanged.AddDynamic(this, &USettingsMenu::OnMouseSensitivityChanged);
	
	ApplyButton->SetIsEnabled(false);
}


void USettingsMenu::NativeDestruct()
{
	Super::NativeDestruct();

	if (ApplyButton->OnClicked.IsBound())
		ApplyButton->OnClicked.RemoveDynamic(this, &USettingsMenu::ApplySensitiveSettings);
	if (ADS_MouseSensitivitySlider->OnValueChanged.IsBound())
		ADS_MouseSensitivitySlider->OnValueChanged.RemoveDynamic(this, &USettingsMenu::OnADSSensitivityChanged);
	if (MouseSensitivitySlider->OnValueChanged.IsBound())
		MouseSensitivitySlider->OnValueChanged.RemoveDynamic(this, &USettingsMenu::OnMouseSensitivityChanged);
}

void USettingsMenu::ApplySensitiveSettings()
{
	const auto character = Cast<ABlasterCharacter>(GetOwningPlayerPawn());
	if (!character) return;
	USensitivitySettings* pSettings = character->GetSensitivitySettings();
	if (!pSettings) return;
	
	//If the difference between the temporary sensitivity and the current sensitivity is not zero, set the new sensitivity
	if (!FMath::IsNearlyZero(m_TemporaryMouseSensitivity - pSettings->GetMouseSensitivityMultiplier()))
		pSettings->SetMouseSensitivity(m_TemporaryMouseSensitivity);
	if (!FMath::IsNearlyZero(m_TemporaryADSSensitivity - pSettings->GetADSSensitivityMultiplier()))
		pSettings->SetADSSensitivity(m_TemporaryADSSensitivity);
	
	m_TemporaryADSSensitivity = 0.0f;
	m_TemporaryMouseSensitivity = 0.0f;
	pSettings->SaveSensitivitySettings();
	ApplyButton->SetIsEnabled(false);
}

void USettingsMenu::OnADSSensitivityChanged(float value)
{
	m_TemporaryADSSensitivity = value;

	//If the value is the same as the current sensitivity, disable the apply button
	if (FMath::IsNearlyZero(m_TemporaryADSSensitivity - USensitivitySettings::LoadSensitivitySettings()->GetADSSensitivityMultiplier()))
	{
		ApplyButton->SetIsEnabled(false);
	}
	else ApplyButton->SetIsEnabled(true);
}

void USettingsMenu::OnMouseSensitivityChanged(float value)
{
	m_TemporaryMouseSensitivity = value;

	//If the value is the same as the current sensitivity, disable the apply button
	if (FMath::IsNearlyZero(m_TemporaryMouseSensitivity - USensitivitySettings::LoadSensitivitySettings()->GetMouseSensitivityMultiplier()))
	{
		ApplyButton->SetIsEnabled(false);
	}
	else ApplyButton->SetIsEnabled(true);
}
