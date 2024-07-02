// Fill out your copyright notice in the Description page of Project Settings.

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
	virtual void OnPossess(APawn* InPawn) override;
	virtual void ReceivedPlayer() override; // This is the earliest we can get the time from the player so we can have an accurate time sync
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetHudHealth(const float health, const float maxHealth);
	void SetHudKills(const float kills);
	void SetHudDeaths(const float deaths);
	void SetHudAmmo(const int ammo);
	void SetHudCarriedAmmo(const int carriedAmmo);
	void ShowAmmo(const bool showAmmo);
	void SetHudMatchCountDown(const float time);
	
	virtual void Tick(float DeltaSeconds) override;
	
	float GetServerTime() const; // Synced with server world time
	void OnMatchStateNameSet(const FName state);
private:
	ABlasterHUD* m_pHUD{};
	void SetHudTime();
	
	// Temporary: to be moved to the game mode
	float m_MatchCountDown{120.f};
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
	void HandleMatchStarted();

	UCharacterOverlay* m_pCharacterOverlay{};
	bool m_InitializedCharacterOverlay{true};

	float m_HudHealth{0.f};
	float m_HudMaxHealth{0.f};
	float m_HudKills{};
	float m_HudDeaths{};
	
	void PollInitialize();
};