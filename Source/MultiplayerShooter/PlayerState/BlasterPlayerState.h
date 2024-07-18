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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Deaths();
	
	void AddToSore(float scoreAmount);
	void AddToDeaths(int deathsAmount = 1);
private:
	UPROPERTY()
	ABlasterCharacter* m_pBlasterCharacter{};
	UPROPERTY()
	ABlasterPlayerController* m_pBlasterPlayerController{};

	UPROPERTY(ReplicatedUsing = OnRep_Deaths)
	int m_Deaths{};	
};
