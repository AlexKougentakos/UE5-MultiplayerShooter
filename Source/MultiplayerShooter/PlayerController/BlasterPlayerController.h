// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

class AWeapon;
class UReturnToMainMenu;
class ABlasterGameMode;
class UCharacterOverlay;
class ABlasterHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, isPingTooHigh);

UCLASS()
class MULTIPLAYERSHOOTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupInputComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnPossess(APawn* InPawn) override;
	void HandleHighPingWarning(float DeltaSeconds);
	virtual void ReceivedPlayer() override; // This is the earliest we can get the time from the player so we can have an accurate time sync
	
	void OnMatchStateSet(const FName state);

	void HighPingWarning();
	void StopHighPingWarning();

	void BroadcastElimination(APlayerState* pAttacker, APlayerState* pVictim, AWeapon* pWeaponUsed);
	
	FHighPingDelegate OnHighPingWarning;

	void ChatOpened() const;
	
	UFUNCTION(Client, Reliable)
	void ClientReceiveChatMessage(const FString& SenderName, const FString& Message);
	
	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& message);
	void SendChatMessage(const FString& Message);

	void UpdateWeaponHud(const AWeapon* pMainWeapon, const AWeapon* pSecondaryWeapon);
	void HideUI(const bool hide);
private:
	UPROPERTY() ABlasterHUD* m_pHUD{};
	UPROPERTY() ABlasterGameMode* m_pGameMode{};

	
	void SetHudTime();
	void PollInitialize();

	//I am following the same convention as the GameMode to handle the match states, defining a handle function for each one
	//Handle Game State Functions
	void HandleMatchHasStarted();
	void HandleCooldown();
	
	float m_MatchDuration{0.f};
	float m_WarmUpDuration{0.f};
	float m_CooldownDuration{0.f};
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
	float m_SingleTripTime{0.f};

	//Every so often we will request the server time to keep the client in sync
	void HandleTimeSync(float DeltaSeconds);

	bool m_CheckPingFlag{false};

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName m_MatchState{};

	UFUNCTION()
	void OnRep_MatchState();

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinedMidGame(FName matchState, float warmUpDuration, float gameDuration, float levelStartingTime, float cooldownDuration);
	
	UFUNCTION(Client, Reliable)
	void ClientEliminationAnnouncement(APlayerState* pAttacker, APlayerState* pVictim, AWeapon* pWeaponUsed);

	
	
	UPROPERTY() UCharacterOverlay* m_pCharacterOverlay{};
	
	UPROPERTY(EditAnywhere, Category = "UI", DisplayName = "Return To Main Menu Widget Class")
	TSubclassOf<UUserWidget> m_ReturnToMainMenuWidgetClass{};
	UPROPERTY() UReturnToMainMenu* m_pReturnToMainMenuWidget{};
	bool m_IsReturnToMainMenuOpen{false};
	void ShowReturnToMainMenu();
	
	float m_TimeSinceLastHighPingWarning{0.f};
	float m_CurrentHighPingAnimationElapsedTime{0.f};
	float m_TimeSinceLastPingUpdate{0.f};
	
	UPROPERTY(EditAnywhere, Category = "Ping", DisplayName = "High Ping Warning Display Time")
	float m_HighPingWarningDisplayTime{5.f};
	UPROPERTY(EditAnywhere, Category = "Ping", DisplayName = "High Ping Check Frequency")
	float m_HighPingCheckFrequency{20.f};
	UPROPERTY(EditAnywhere, Category = "Ping", DisplayName = "High Ping Threshold")
	float m_HighPingThreshold{100.f};
	UPROPERTY(EditAnywhere, Category = "Ping", DisplayName = "Server Side Rewind Ping Cap")
	float m_ServerSideRewindPingCap{150.f};
	UPROPERTY(EditAnywhere, Category = "Ping", DisplayName = "Ping Update Frequency")
	float m_PingUpdateFrequency{1.f};

	// Values to cache
	float m_Health{0.f};
	float m_MaxHealth{0.f};
	bool m_InitializeHealth{false};
	float m_Kills{0.f};
	bool m_InitializeKills{false};
	float m_Deaths{0.f};
	bool m_InitializeDeaths{false};
	float m_Shield{0.f};
	float m_MaxShield{0.f};
	bool m_InitializeShield{false};
	float m_CarriedAmmo{0.f};
	bool m_InitializeCarriedAmmo{false};
	int m_WeaponAmmo{0};
	bool m_InitializeWeaponAmmo{false};
	bool m_InitializeWeaponHUD{false};
	const AWeapon* m_pMainWeapon{};
	const AWeapon* m_pSecondaryWeapon{};
public:
		
	void SetHudHealth(const float health, const float maxHealth);
	void SetHudShield(const float shield, const float maxShield);
	void SetHudScore(const float score);
	void SetHudDeaths(const float deaths);
	void SetHudAmmo(const int ammo);
	void SetHudCarriedAmmo(const int carriedAmmo);
	void SetHudMatchCountDown(const float time);
	void SetHudAnnouncementCountDown(const float time);
	
	float GetSingleTripTime() const { return m_SingleTripTime; }
	float GetServerTime() const; // Synced with server world time

	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool highPing);

	void MarkPingCheckFlag() { m_CheckPingFlag = true; }
};

