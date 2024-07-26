#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Chat.generated.h"

class UVerticalBox;
class UHorizontalBox;
class UCanvasPanel;
class UImage;
class UEditableTextBox;
class UScrollBox;
class UChatMessage;

UCLASS()
class MULTIPLAYERSHOOTER_API UChat : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	
	void NativeConstruct() override;
	void OpenChat() const;
	void CloseChat() const;

	void AddChatMessage(const FString& senderName, const FString& message);

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
private:
	UFUNCTION()
	void OnChatMessageCommitted(const FText& text, ETextCommit::Type commitMethod);

	UChatMessage* CreateChatMessage(const FString& senderName, const FString& message) const;
	void AddMessageToContainer(const FString& senderName, const FString& message, UPanelWidget* pContainer, const int maxElements);
	
	UPROPERTY(EditAnywhere, Category = "Chat", DisplayName = "Chat Message Class")
	TSubclassOf<UChatMessage> m_pChatMessageClass{};

	UPROPERTY(meta = (BindWidget))
	UScrollBox* MessageContainer{};

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* CurrentChatMessage{};

	UPROPERTY(meta = (BindWidget))
	UImage* UnderlineImage{};

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* ChatArea{};

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* ChatInputContainer{};
	
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* RecentMessages{};

	UPROPERTY(EditAnywhere, Category = "Chat", DisplayName = "Recent Message Cap")
	int m_RecentMessageCap{5};

	UPROPERTY(EditAnywhere, Category = "Chat", DisplayName = "Message History Cap")
	int m_MessageHistoryCap{30};
};
