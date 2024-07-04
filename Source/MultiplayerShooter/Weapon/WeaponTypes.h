#pragma once


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Rifle		UMETA(DisplayName = "Rifle"),
	EWT_RocketLauncher	UMETA(DisplayName = "Rocket Launcher"),


	
	EWT_MAX			UMETA(DisplayName = "DefaultMAX") // Keep in the end
};
