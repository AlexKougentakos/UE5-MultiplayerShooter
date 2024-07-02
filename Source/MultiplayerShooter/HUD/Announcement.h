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
	UTextBlock* TimerText{};
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AnnouncementText{};
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoText{};
	
};
