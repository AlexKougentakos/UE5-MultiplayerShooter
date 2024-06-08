// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadWidget.generated.h"

class UTextBlock;
UCLASS()
class MULTIPLAYERSHOOTER_API UOverHeadWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayText{};

	void SetDisplayText(const FString& text) const;

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(const APawn* const pawn) const;
	
protected:
	virtual void NativeDestruct() override;
};
