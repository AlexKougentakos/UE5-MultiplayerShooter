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
	static USensitivitySettings* StaticSettings = nullptr;
    
	if (!StaticSettings)
	{
		StaticSettings = Cast<USensitivitySettings>(UGameplayStatics::LoadGameFromSlot("SensitivitySettings", 0));
        
		if (!StaticSettings)
		{
			StaticSettings = Cast<USensitivitySettings>(UGameplayStatics::CreateSaveGameObject(USensitivitySettings::StaticClass()));
		}
	}
    
	return StaticSettings;
}

void USensitivitySettings::SaveSensitivitySettings()
{
	OnSensitivitySettingsChanged.Broadcast(m_MouseSensitivityMultiplier, m_ADSSensitivityMultiplier);
	UGameplayStatics::SaveGameToSlot(this, "SensitivitySettings", 0);
}