// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatMessage.generated.h"

class UTextBlock;

UCLASS()
class MULTIPLAYERSHOOTER_API UChatMessage : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetChatMessage(const FString& senderName, const FString& message) const;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SenderName{};
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SenderMessage{};
};
