// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UChat;
class UVerticalBox;
class UImage;
class UHorizontalBox;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText{};

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShieldText{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathsAmount{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoCount{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoCount{};
	
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* AmmoContainer{};
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountDownText{};

	UPROPERTY(meta = (BindWidget))
	UImage* PingWarning{};

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* PingWarningAnimation{};
	
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* KillFeed{};
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PingText{};

	UPROPERTY(meta = (BindWidget))
	UChat* Chat{};

	/*
	 *	BOTTOM RIGHT WEAPON CONTAINER
	 */

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponIcon{};
	
	UPROPERTY(meta = (BindWidget))
	UImage* SecondaryWeaponIcon{};
	
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* SecondaryWeapon{};

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* BottomContainer{};
};
