#include "Chat.h"
#include "ChatMessage.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/PlayerState.h"

void UChat::NativeConstruct()
{
	Super::NativeConstruct();

	CloseChat();
	CurrentChatMessage->OnTextCommitted.AddDynamic(this, &UChat::OnChatMessageCommitted);	
}

void UChat::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!CurrentChatMessage->HasKeyboardFocus() && CurrentChatMessage->GetText().IsEmpty())
	{
		CloseChat();
	}
}

void UChat::OpenChat() const
{	
	GetOwningPlayer()->SetInputMode(FInputModeUIOnly());
	GetOwningPlayer()->bShowMouseCursor = true;
	
	ChatArea->SetVisibility(ESlateVisibility::Visible);
	UnderlineImage->SetVisibility(ESlateVisibility::Visible);
	RecentMessages->SetVisibility(ESlateVisibility::Hidden);

	UVerticalBoxSlot* pChatInputContainerSlot = Cast<UVerticalBoxSlot>(RecentMessages->Slot);
	FSlateChildSize test{ESlateSizeRule::Fill};
	test.Value = 0.5f;
	
	pChatInputContainerSlot->SetSize(test);
	CurrentChatMessage->SetUserFocus(GetOwningPlayer());
	CurrentChatMessage->SetKeyboardFocus();
	CurrentChatMessage->SetText(FText{});
}

void UChat::CloseChat() const
{
	GetOwningPlayer()->SetInputMode(FInputModeGameOnly());
	GetOwningPlayer()->bShowMouseCursor = false;
	
	ChatArea->SetVisibility(ESlateVisibility::Hidden);
	UnderlineImage->SetVisibility(ESlateVisibility::Hidden);
	RecentMessages->SetVisibility(ESlateVisibility::Visible);

	UVerticalBoxSlot* pChatInputContainerSlot = Cast<UVerticalBoxSlot>(RecentMessages->Slot);
	FSlateChildSize test{ESlateSizeRule::Fill};
	test.Value = 9999.f;
	
	pChatInputContainerSlot->SetSize(test);
	
	CurrentChatMessage->SetText(FText{FText::FromString("Press \"/\" to type...")});

}

void UChat::AddChatMessage(const FString& senderName, const FString& message)
{
	//You have to create two instances because an element cannot have two parents
	RecentMessages->AddChild(CreateChatMessage(senderName, message));
	MessageContainer->AddChild(CreateChatMessage(senderName, message));
	
	if (RecentMessages->GetChildrenCount() > m_RecentMessageCap)
	{
		RecentMessages->RemoveChildAt(0);
	}
}


UChatMessage* UChat::CreateChatMessage(const FString& senderName, const FString& message) const
{
	UChatMessage* pChatMessage = CreateWidget<UChatMessage>(GetOwningPlayer(), m_pChatMessageClass);
	pChatMessage->SetChatMessage(senderName, message);
	//Rotate the object 180 degrees
	//This is a work around recommended by the Unreal Engine documentation
	//https://forums.unrealengine.com/t/reverse-order-of-scrollbox-widget/407588
	pChatMessage->SetRenderTransformAngle(180.f);

	return pChatMessage;
}


void UChat::OnChatMessageCommitted(const FText& text, ETextCommit::Type commitMethod)
{
	if (commitMethod != ETextCommit::OnEnter || text.IsEmpty()) return;
	
	AddChatMessage(GetOwningPlayer()->GetPlayerState<APlayerState>()->GetPlayerName(), text.ToString());
	CloseChat();
}
