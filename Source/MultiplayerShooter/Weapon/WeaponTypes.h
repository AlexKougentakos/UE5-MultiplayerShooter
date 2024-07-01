#pragma once


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Rifle		UMETA(DisplayName = "Rifle"),
	EWT_Pistol		UMETA(DisplayName = "Pistol"),
	EWT_Shotgun		UMETA(DisplayName = "Shotgun"),

	
	EWT_MAX			UMETA(DisplayName = "DefaultMAX") // Keep in the end
};
