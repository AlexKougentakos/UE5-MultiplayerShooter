#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EliminationAnnouncement.generated.h"

class AWeapon;
class UImage;
class UTextBlock;
class UHorizontalBox;

UCLASS()
class MULTIPLAYERSHOOTER_API UEliminationAnnouncement : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetEliminationAnnouncement(const FString& eliminator, const FString& eliminated, const AWeapon* weapon);

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* AnnouncementBox{};
	UPROPERTY(meta = (BindWidget))
	UTextBlock* EliminatorPlayer{};
	UPROPERTY(meta = (BindWidget))
	UTextBlock* EliminatedPlayer{};
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponImage{};
};
