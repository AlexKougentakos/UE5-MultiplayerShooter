// Fill out your copyright notice in the Description page of Project Settings.


#include "EliminationAnnouncement.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MultiplayerShooter/Weapon/Weapon.h"

void UEliminationAnnouncement::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	m_DisplayTime -= InDeltaTime;
	if (m_DisplayTime <= 0.f)
	{
		RemoveFromParent();
	}
}

void UEliminationAnnouncement::SetEliminationAnnouncement(const FString& eliminator, const FString& eliminated,
                                                          const AWeapon* weapon)
{
	EliminatedPlayer->SetText(FText::FromString(eliminated));
	EliminatorPlayer->SetText(FText::FromString(eliminator));
	UTexture2D* weaponIcon = weapon->GetKillfeedIcon();
	FSlateBrush brush;
	brush.ImageSize = FVector2D(weaponIcon->GetSizeX(), weaponIcon->GetSizeY());
	brush.SetResourceObject(weaponIcon);
	WeaponImage->SetBrush(brush);
}
