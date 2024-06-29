// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

class UCharacterOverlay;

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

	float CrosshairSpread{};
	FLinearColor CrosshairColor{FLinearColor::White};
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

	UPROPERTY(EditAnywhere, Category = "Player Stats", DisplayName = "Character Overlay Class")
	TSubclassOf<UUserWidget> m_pCharacterOverlayClass{};
	
	UCharacterOverlay* m_pCharacterOverlay{};
	
	virtual void BeginPlay() override;
private:
	FHUDPackage m_HUDPackage{};

	UPROPERTY(EditAnywhere, Category = "Crosshairs", DisplayName = "Crosshair Spread Max")
	float m_CrosshairSpreadMax{8.f};

	void DrawCrosshair(UTexture2D* crosshair, const FVector2D& viewportCenter, const FVector2D& spread, const FLinearColor& color);
	void AddCharacterOverlay();
public: // Getters & Setters
	void SetHudPackage(const FHUDPackage& hudPackage) { m_HUDPackage = hudPackage; }

	
};
