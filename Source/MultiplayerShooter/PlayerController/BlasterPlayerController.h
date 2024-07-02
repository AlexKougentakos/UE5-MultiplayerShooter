﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

class UCharacterOverlay;
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void ReceivedPlayer() override; // This is the earliest we can get the time from the player so we can have an accurate time sync
	
	void SetHudHealth(const float health, const float maxHealth);
	void SetHudScore(const float score);
	void SetHudDeaths(const float deaths);
	void SetHudAmmo(const int ammo);
	void SetHudCarriedAmmo(const int carriedAmmo);
	void ShowAmmo(const bool showAmmo);
	void SetHudMatchCountDown(const float time);
	void SetHudAnnouncementCountDown(const float time);
	
	virtual void Tick(float DeltaSeconds) override;
	
	float GetServerTime() const; // Synced with server world time

	void OnMatchStateSet(const FName state);


private:
	ABlasterHUD* m_pHUD{};
	void SetHudTime();
	void PollInitialize();
	void HandleMatchHasStarted();
	
	float m_MatchDuration{0.f};
	float m_WarmUpDuration{0.f};
	float m_LevelStartingTime{0.f};
	
	unsigned int m_CountDownSeconds{0};

	/*
	 *	SYNC TIME BETWEEN SERVER & CLIENTS
	 */
	// Request the current server time from the server
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float timeOfClientRequest);

	// Reports the current server time to the client in response to the ServerRquestTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float timeOfClientRequest, float timeServerReceivedRequest);

	float m_ClientServerTimeDifference{0.f};

	UPROPERTY(EditAnywhere, Category = "Time Sync", DisplayName = "Time Sync Frequency")
	float m_TimeSyncFrequency{5.f};

	float m_TimeSinceLastSync{0.f};

	//Every so often we will request the server time to keep the client in sync
	void HandleTimeSync(float DeltaSeconds);

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName m_MatchState{};

	UFUNCTION()
	void OnRep_MatchState();

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinedMidGame(FName matchState, float warmUpDuration, float gameDuration, float levelStartingTime);
	
	UCharacterOverlay* m_pCharacterOverlay{};
	bool m_InitializeCharacterOverlay{false};

	// Values to cache
	float m_Health{0.f};
	float m_MaxHealth{0.f};
	float m_Kills{0.f};
	float m_Deaths{0.f};
};

