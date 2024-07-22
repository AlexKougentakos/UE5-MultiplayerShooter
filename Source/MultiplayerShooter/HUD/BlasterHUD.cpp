// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

#include "Announcement.h"
#include "CharacterOverlay.h"
#include "Chat.h"
#include "EliminationAnnouncement.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "MultiplayerShooter/Weapon/Weapon.h"

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

void ABlasterHUD::AddEliminationAnnouncement(const FString& eliminatedPlayer, const FString& eliminatorPlayer, const AWeapon* pWeaponUsed)
{
	m_pPlayerController = GetOwningPlayerController();
	checkf(m_pPlayerController, TEXT("Player controller is null"));
	checkf(m_EliminationAnnouncementClass, TEXT("Elimination announcement class is null"));

	UEliminationAnnouncement* pEliminationAnnouncement = CreateWidget<UEliminationAnnouncement>(m_pPlayerController, m_EliminationAnnouncementClass);
	pEliminationAnnouncement->SetDisplayTime(m_KillFeedItemDisplayTime);
	pEliminationAnnouncement->SetEliminationAnnouncement(eliminatorPlayer, eliminatedPlayer, pWeaponUsed);

	//Get all the current children
	TArray<UWidget*> killFeedItems = m_pCharacterOverlay->KillFeed->GetAllChildren();
	//Add it at the beginning, otherwise it would appear at the bottom of the list
	killFeedItems.EmplaceAt(0, pEliminationAnnouncement);
	
	m_pCharacterOverlay->KillFeed->ClearChildren();
	for (UWidget* pItem : killFeedItems)
	{
		m_pCharacterOverlay->KillFeed->AddChildToVerticalBox(pItem);
	}

	if (m_pCharacterOverlay->KillFeed->GetChildrenCount() > m_MaxKillFeedItems)
	{
		m_pCharacterOverlay->KillFeed->RemoveChildAt(m_MaxKillFeedItems);
	}
}

void ABlasterHUD::AddChatMessage(const FString& senderName, const FString& message) const
{
	m_pCharacterOverlay->Chat->AddChatMessage(senderName, message);
}


void ABlasterHUD::ChatOpened() const
{
	if (m_pCharacterOverlay && m_pCharacterOverlay->Chat)
	{
		m_pCharacterOverlay->Chat->OpenChat();
	}
}

void ABlasterHUD::UpdateWeaponHud(const AWeapon* pMainWeapon, const AWeapon* pSecondaryWeapon) const
{
	if (!m_pCharacterOverlay) return;

	if (pMainWeapon)
		m_pCharacterOverlay->WeaponIcon->SetBrushFromTexture(pMainWeapon->GetKillfeedIcon());
	
	if (pSecondaryWeapon)
	{
		m_pCharacterOverlay->SecondaryWeapon->SetVisibility(ESlateVisibility::Visible);
		m_pCharacterOverlay->SecondaryWeaponIcon->SetBrushFromTexture(pSecondaryWeapon->GetKillfeedIcon());
	}
	else
		m_pCharacterOverlay->SecondaryWeapon->SetVisibility(ESlateVisibility::Hidden);
}

void ABlasterHUD::HideUI(const bool hide) const
{
	m_pCharacterOverlay->SetVisibility(hide ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}
