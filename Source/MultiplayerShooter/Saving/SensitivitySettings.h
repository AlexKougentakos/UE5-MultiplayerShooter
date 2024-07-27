#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SensitivitySettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSensitivitySettingsChanged, float, MouseSensitivity, float, ADSSensitivity);

UCLASS()
class MULTIPLAYERSHOOTER_API USensitivitySettings : public USaveGame
{
	GENERATED_BODY()

public:
	USensitivitySettings();
	
	// Load and Save functions
	UFUNCTION()
	static USensitivitySettings* GetSensitivitySettings();
    
	UFUNCTION(BlueprintCallable, Category = "Sensitivity")
	void SaveSensitivitySettings();

	UPROPERTY(BlueprintAssignable, Category = "Sensitivity")
	FOnSensitivitySettingsChanged OnSensitivitySettingsChanged;

	// Debug function
	void LogDelegateInfo() const;
    
private:
    float m_MouseSensitivityMultiplier;
    float m_ADSSensitivityMultiplier;
    
public: // Getters & Setters
	UFUNCTION(BlueprintCallable, Category = "Sensitivity")
	float GetMouseSensitivityMultiplier() const { return m_MouseSensitivityMultiplier; }

	UFUNCTION(BlueprintCallable, Category = "Sensitivity")
	float GetADSSensitivityMultiplier() const { return m_ADSSensitivityMultiplier; }
    
	UFUNCTION(BlueprintCallable, Category = "Sensitivity")
	void SetMouseSensitivity(float NewSensitivity);

	UFUNCTION(BlueprintCallable, Category = "Sensitivity")
	void SetADSSensitivity(float NewSensitivity);
};
