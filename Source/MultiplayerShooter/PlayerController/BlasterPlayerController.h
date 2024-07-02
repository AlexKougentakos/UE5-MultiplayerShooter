// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

class ABlasterHUD;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
		
	void SetHudHealth(const float health, const float maxHealth);
	void SetHudScore(const float score);
	void SetHudDeaths(const float deaths);
	void SetHudAmmo(const int ammo);
	void SetHudCarriedAmmo(const int carriedAmmo);
	void ShowAmmo(const bool showAmmo);
	void SetHudMatchCountDown(const float time);

	virtual void Tick(float DeltaSeconds) override;
private:
	ABlasterHUD* m_pHUD{};
	void SetHudTime();
	
	// Temporary: to be moved to the game mode
	float m_MatchCountDown{120.f};
	unsigned int m_CountDownSeconds{0};
};
