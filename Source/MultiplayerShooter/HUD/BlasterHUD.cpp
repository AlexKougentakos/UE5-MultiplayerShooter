// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D viewportCenter = FVector2D(viewportSize.X / 2, viewportSize.Y / 2);

	const float spreadAmount = m_CrosshairSpreadMax * m_HUDPackage.CrosshairSpread;
	
	if (m_HUDPackage.CrosshairsCenter) DrawCrosshair(m_HUDPackage.CrosshairsCenter, viewportCenter, FVector2D(0.f, 0.f), m_HUDPackage.CrosshairColor);
	if (m_HUDPackage.CrosshairsLeft) DrawCrosshair(m_HUDPackage.CrosshairsLeft, viewportCenter, FVector2D(-spreadAmount, 0.f), m_HUDPackage.CrosshairColor);
	if (m_HUDPackage.CrosshairsRight) DrawCrosshair(m_HUDPackage.CrosshairsRight, viewportCenter, FVector2D(spreadAmount, 0.f), m_HUDPackage.CrosshairColor);
	if (m_HUDPackage.CrosshairsTop) DrawCrosshair(m_HUDPackage.CrosshairsTop, viewportCenter, FVector2D(0.f, -spreadAmount), m_HUDPackage.CrosshairColor);
	if (m_HUDPackage.CrosshairsBottom) DrawCrosshair(m_HUDPackage.CrosshairsBottom, viewportCenter, FVector2D(0.f, spreadAmount), m_HUDPackage.CrosshairColor);
	
}

void ABlasterHUD::DrawCrosshair(UTexture2D* crosshair, const FVector2D& viewportCenter, const FVector2D& spread, const FLinearColor& color)
{
	const float textureWidth = crosshair->GetSizeX();
	const float textureHeight = crosshair->GetSizeY();

	const FVector2D textureCenter = {viewportCenter.X - textureWidth / 2.f + spread.X, viewportCenter.Y - textureHeight / 2.f + spread.Y};

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
