// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

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
	UTextBlock* ScoreAmount{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathsAmount{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoCount{};

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* AmmoContainer{};
};
