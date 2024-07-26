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
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"

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
    AddMessageToContainer(senderName, message, MessageContainer, m_MessageHistoryCap);
    AddMessageToContainer(senderName, message, RecentMessages, m_RecentMessageCap);
}

void UChat::AddMessageToContainer(const FString& senderName, const FString& message, UPanelWidget* pContainer, const int maxElements)
{
    //You have to create two instances because an element cannot have two parents
    //Hence why this is inside this function
    UWidget* pNewChatMessage = CreateChatMessage(senderName, message);

    TArray<UWidget*> pTemporaryChildrenContainer = pContainer->GetAllChildren();
    pTemporaryChildrenContainer.Insert(pNewChatMessage, 0);
    pContainer->ClearChildren();

    if (pTemporaryChildrenContainer.Num() > maxElements)
    {
        pTemporaryChildrenContainer.RemoveAt(pTemporaryChildrenContainer.Num() - 1);
    }
    
    for (int i = 0; i < pTemporaryChildrenContainer.Num(); i++)
    {
        pContainer->AddChild(pTemporaryChildrenContainer[i]);
    }
}

UChatMessage* UChat::CreateChatMessage(const FString& senderName, const FString& message) const
{
    UChatMessage* pChatMessage = CreateWidget<UChatMessage>(GetOwningPlayer(), m_pChatMessageClass);
    pChatMessage->SetChatMessage(senderName, message);
    //Rotate the object 180 degrees
    //This is a work around recommended by the Unreal Engine documentation
    //https://forums.unrealengine.com/t/reverse-order-of-scrollbox-widget/407588
    //pChatMessage->SetRenderTransformAngle(180.f);

    //Set the scale to -1
    // Workaround 2
    // https://forums.unrealengine.com/t/is-it-possible-to-reverse-the-vertical-orientation-of-an-umg-scroll-box/91917/5
    //This makes it so that the scroll bar is at-least at the correct side, the scrolling is still reversed though :(
    pChatMessage->SetRenderScale({1, -1});

    return pChatMessage;
}

void UChat::OnChatMessageCommitted(const FText& text, ETextCommit::Type commitMethod)
{
    if (commitMethod != ETextCommit::OnEnter || text.IsEmpty()) return;
    
    if (ABlasterPlayerController* pPlayerController = Cast<ABlasterPlayerController>(GetOwningPlayer()))
    {
        pPlayerController->ServerSendChatMessage(text.ToString());
    }
    CloseChat();
}
