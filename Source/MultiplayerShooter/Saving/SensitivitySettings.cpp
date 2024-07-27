// SensitivitySettings.cpp

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

USensitivitySettings* USensitivitySettings::GetSensitivitySettings()
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
    LogDelegateInfo(); // Log delegate info before broadcasting

    if (OnSensitivitySettingsChanged.IsBound())
    {
        UE_LOG(LogTemp, Warning, TEXT("Broadcasting sensitivity changes: Mouse=%f, ADS=%f"), 
               m_MouseSensitivityMultiplier, m_ADSSensitivityMultiplier);
        OnSensitivitySettingsChanged.Broadcast(m_MouseSensitivityMultiplier, m_ADSSensitivityMultiplier);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnSensitivitySettingsChanged is not bound. Cannot broadcast."));
    }

    UGameplayStatics::SaveGameToSlot(this, "SensitivitySettings", 0);
}

void USensitivitySettings::LogDelegateInfo() const
{
    UE_LOG(LogTemp, Warning, TEXT("Delegate Info - IsBound: %s, Num Functions: %d"), 
           OnSensitivitySettingsChanged.IsBound() ? TEXT("True") : TEXT("False"),
           OnSensitivitySettingsChanged.GetAllObjects().Num());
}