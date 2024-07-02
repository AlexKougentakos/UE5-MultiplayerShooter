// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

#include "Announcement.h"
#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D viewportCenter = FVector2D(viewportSize.X / 2, viewportSize.Y / 2);

	const float spreadAmount = m_CrosshairSpreadMax * m_HUDPackage.CrosshairSpread;

	if (m_HUDPackage.CrosshairsCenter) DrawCrosshair(m_HUDPackage.CrosshairsCenter, viewportCenter, FVector2D(0.f, 0.f),
	                                                 m_HUDPackage.CrosshairColor);
	if (m_HUDPackage.CrosshairsLeft) DrawCrosshair(m_HUDPackage.CrosshairsLeft, viewportCenter,
	                                               FVector2D(-spreadAmount, 0.f), m_HUDPackage.CrosshairColor);
	if (m_HUDPackage.CrosshairsRight) DrawCrosshair(m_HUDPackage.CrosshairsRight, viewportCenter,
	                                                FVector2D(spreadAmount, 0.f), m_HUDPackage.CrosshairColor);
	if (m_HUDPackage.CrosshairsTop) DrawCrosshair(m_HUDPackage.CrosshairsTop, viewportCenter,
	                                              FVector2D(0.f, -spreadAmount), m_HUDPackage.CrosshairColor);
	if (m_HUDPackage.CrosshairsBottom) DrawCrosshair(m_HUDPackage.CrosshairsBottom, viewportCenter,
	                                                 FVector2D(0.f, spreadAmount), m_HUDPackage.CrosshairColor);
}

void ABlasterHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ABlasterHUD::DrawCrosshair(UTexture2D* crosshair, const FVector2D& viewportCenter, const FVector2D& spread,
                                const FLinearColor& color)
{
	const float textureWidth = crosshair->GetSizeX();
	const float textureHeight = crosshair->GetSizeY();

	const FVector2D textureCenter = {
		viewportCenter.X - textureWidth / 2.f + spread.X, viewportCenter.Y - textureHeight / 2.f + spread.Y
	};

	DrawTexture(
		crosshair,
		textureCenter.X,
		textureCenter.Y,
		textureWidth,
		textureHeight,
		0.f, 0.f,
		1.f, 1.f,
		color);
}

void ABlasterHUD::AddAnnouncement()
{
	const auto pPlayerController = GetOwningPlayerController();
	checkf(pPlayerController, TEXT("Player controller is null"));
	checkf(m_pWarmupAnnouncementClass, TEXT("Character overlay class is null"));

	m_pWarmupAnnouncement = CreateWidget<UAnnouncement>(pPlayerController, m_pWarmupAnnouncementClass);
	m_pWarmupAnnouncement->AddToViewport();
}

void ABlasterHUD::AddCharacterOverlay()
{
	const auto pPlayerController = GetOwningPlayerController();
	checkf(pPlayerController, TEXT("Player controller is null"));
	checkf(m_pCharacterOverlayClass, TEXT("Character overlay class is null"));

	m_pCharacterOverlay = CreateWidget<UCharacterOverlay>(pPlayerController, m_pCharacterOverlayClass);
	m_pCharacterOverlay->AddToViewport();
}

void ABlasterHUD::AddAnnouncement()
{
	const auto pPlayerController = GetOwningPlayerController();
	checkf(pPlayerController, TEXT("Player controller is null"));
	checkf(m_pAnnouncementClass, TEXT("Announcement class is null"));

	m_pAnnouncement = CreateWidget<UAnnouncement>(pPlayerController, m_pAnnouncementClass);
	m_pAnnouncement->AddToViewport();
}
