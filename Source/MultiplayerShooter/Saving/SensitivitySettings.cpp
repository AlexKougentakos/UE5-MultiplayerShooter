#include "SensitivitySettings.h"
#include "Kismet/GameplayStatics.h"

USensitivitySettings::USensitivitySettings()
	: m_MouseSensitivityMultiplier(1.0f)
	, m_ADSSensitivityMultiplier(0.5f)
{
}

void USensitivitySettings::SetMouseSensitivity(float NewSensitivity)
{
	m_MouseSensitivityMultiplier = FMath::Clamp(NewSensitivity, 0.1f, 10.0f);
}

void USensitivitySettings::SetADSSensitivity(float NewSensitivity)
{
	m_ADSSensitivityMultiplier = FMath::Clamp(NewSensitivity, 0.1f, 10.0f);
}

USensitivitySettings* USensitivitySettings::LoadSensitivitySettings()
{    
	USensitivitySettings* Settings = Cast<USensitivitySettings>(UGameplayStatics::LoadGameFromSlot("SensitivitySettings", 0));
    
	if (!Settings)
	{
		Settings = Cast<USensitivitySettings>(UGameplayStatics::CreateSaveGameObject(USensitivitySettings::StaticClass()));
		UGameplayStatics::SaveGameToSlot(Settings, "SensitivitySettings", 0);
	}
    
	return Settings;
}

void USensitivitySettings::SaveSensitivitySettings()
{
	if (OnSensitivitySettingsChanged.IsBound())
		OnSensitivitySettingsChanged.Broadcast(m_MouseSensitivityMultiplier, m_ADSSensitivityMultiplier);
	UGameplayStatics::SaveGameToSlot(this, "SensitivitySettings", 0);
}