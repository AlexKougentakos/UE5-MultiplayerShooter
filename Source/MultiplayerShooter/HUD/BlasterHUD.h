// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

class UAnnouncement;
class UCharacterOverlay;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	UPROPERTY() UTexture2D* CrosshairsCenter{};
	UPROPERTY() UTexture2D* CrosshairsLeft{};
	UPROPERTY() UTexture2D* CrosshairsRight{};
	UPROPERTY() UTexture2D* CrosshairsTop{};
	UPROPERTY() UTexture2D* CrosshairsBottom{};

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
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "UI", DisplayName = "Character Overlay Class")
	TSubclassOf<UUserWidget> m_pCharacterOverlayClass{};

	UPROPERTY(EditAnywhere, Category = "UI", DisplayName = "Announcement Class")
	TSubclassOf<UUserWidget> m_pAnnouncementClass{};
	
	UPROPERTY() UCharacterOverlay* m_pCharacterOverlay{};
	UPROPERTY() UAnnouncement* m_pAnnouncement{};
	
	void AddCharacterOverlay();
	void AddAnnouncement();
private:
	FHUDPackage m_HUDPackage{};

	UPROPERTY(EditAnywhere, Category = "Crosshairs", DisplayName = "Crosshair Spread Max")
	float m_CrosshairSpreadMax{8.f};

	void DrawCrosshair(UTexture2D* crosshair, const FVector2D& viewportCenter, const FVector2D& spread, const FLinearColor& color);
public: // Getters & Setters
	void SetHudPackage(const FHUDPackage& hudPackage) { m_HUDPackage = hudPackage; }

	
};
