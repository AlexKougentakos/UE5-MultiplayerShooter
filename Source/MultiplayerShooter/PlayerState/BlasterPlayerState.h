// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

class ABlasterPlayerController;
class ABlasterCharacter;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void OnRep_Score() override;
	void AddToSore(float scoreAmount);
private:
	ABlasterCharacter* m_pBlasterCharacter{};
	ABlasterPlayerController* m_pBlasterPlayerController{};
	
};
