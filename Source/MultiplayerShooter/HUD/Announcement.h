// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Announcement.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API UAnnouncement : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AnnouncementTimer{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AnnouncementTitle{};
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AnnouncementDescription{};
};
