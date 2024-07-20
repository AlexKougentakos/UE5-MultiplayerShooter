#include "ChatMessage.h"

#include "Components/TextBlock.h"

void UChatMessage::SetChatMessage(const FString& senderName, const FString& message) const
{
	const FString formattedName = FString::Printf(TEXT("[%s]: "), *senderName);
	SenderName->SetText(FText::FromString(formattedName));
	SenderMessage->SetText(FText::FromString(message));
}