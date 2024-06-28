// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	UTexture2D* CrosshairsCenter{};
	UTexture2D* CrosshairsLeft{};
	UTexture2D* CrosshairsRight{};
	UTexture2D* CrosshairsTop{};
	UTexture2D* CrosshairsBottom{};
};

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

private:
	FHUDPackage m_HUDPackage{};

	void DrawCrosshair(UTexture2D* crosshair, const FVector2D& viewportCenter);
public: // Getters & Setters
	void SetHudPackage(const FHUDPackage& hudPackage) { m_HUDPackage = hudPackage; }
};
