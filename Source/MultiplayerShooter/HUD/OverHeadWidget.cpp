// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverHeadWidget::SetDisplayText(const FString& text) const
{
	if (!DisplayText) return;

	DisplayText->SetText(FText::FromString(text));
}

void UOverHeadWidget::ShowPlayerNetRole(const APawn* const pawn) const
{
	if (!pawn) return;

	const ENetRole localNetRole = pawn->GetLocalRole();
	
	const FString role = [&]() -> FString
	{
		switch (localNetRole)
		{
		case ENetRole::ROLE_Authority:
			return "Authority";
		case ENetRole::ROLE_AutonomousProxy:
			return "Autonomous Proxy";
		case ENetRole::ROLE_SimulatedProxy:
			return  "Simulated Proxy";
		case ENetRole::ROLE_None:
			return "None";
		default:
			return "";
		}
	}();
	

	//set the display text to the player name and the role
	const FString localRoleString = FString::Printf(TEXT("%s: %s"), *pawn->GetPlayerState()->GetPlayerName(), *role);
	SetDisplayText(localRoleString);
}

void UOverHeadWidget::NativeDestruct()
{
	RemoveFromParent();
	
	Super::NativeDestruct();
}
