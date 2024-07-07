#pragma once

#define BULLET_TRACE_LENGTH 100'000.f

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Rifle		UMETA(DisplayName = "Rifle"),
	EWT_RocketLauncher	UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol		UMETA(DisplayName = "Pistol"),
	EWT_SubmachineGun	UMETA(DisplayName = "Submachine Gun"),
	EWT_Sniper		UMETA(DisplayName = "Sniper"),
	EWT_Shotgun		UMETA(DisplayName = "Shotgun"),
	EWT_GrenadeLauncher	UMETA(DisplayName = "Grenade Launcher"),
	
	
	EWT_MAX			UMETA(DisplayName = "DefaultMAX") // Keep in the end
};