﻿#include "BlasterPlayerController.h"

#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/GameModes/BlasterGameMode.h"
#include "MultiplayerShooter/HUD/Announcement.h"
#include "MultiplayerShooter/HUD/BlasterHUD.h"
#include "MultiplayerShooter/HUD/CharacterOverlay.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController()) ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	
}

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	m_pHUD = Cast<ABlasterHUD>(GetHUD());
	ServerCheckMatchState();
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, m_MatchState);
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const ABlasterCharacter* pCharacter = Cast<ABlasterCharacter>(InPawn);
	if (!pCharacter) return;

	SetHudHealth(pCharacter->GetCurrentHealth(), pCharacter->GetMaxHealth());
}

void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHudTime();
	
	HandleTimeSync(DeltaSeconds);
	PollInitialize();
}


void ABlasterPlayerController::HandleTimeSync(float DeltaSeconds)
{
	m_TimeSinceLastSync += DeltaSeconds;
	if (m_TimeSinceLastSync >= m_TimeSyncFrequency && IsLocalController())
	{
		m_TimeSinceLastSync = 0.f;
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::PollInitialize()
{
	if (!m_pCharacterOverlay)
	{
		if (m_pHUD && m_pHUD->m_pCharacterOverlay)
		{
			m_pCharacterOverlay = m_pHUD->m_pCharacterOverlay;

			// Initialize the HUD values once the overlay is valid
			SetHudHealth(m_Health, m_MaxHealth);
			SetHudScore(m_Kills);
			SetHudDeaths(m_Deaths);
			ShowAmmo(false);
		}
	}
}

void ABlasterPlayerController::SetHudHealth(const float health, const float maxHealth)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());

	if (!m_pHUD ||
		!m_pHUD->m_pCharacterOverlay ||
		!m_pHUD->m_pCharacterOverlay->HealthBar ||
		!m_pHUD->m_pCharacterOverlay->HealthText)
	{
		m_InitializeCharacterOverlay = true;
		m_Health = health;
		m_MaxHealth = maxHealth;
		return;
	}

	// Set health bar
	const float healthPercentage = health / maxHealth;
	m_pHUD->m_pCharacterOverlay->HealthBar->SetPercent(healthPercentage);

	// Set health text
	const FString healthString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(health), FMath::CeilToInt(maxHealth));
	m_pHUD->m_pCharacterOverlay->HealthText->SetText(FText::FromString(healthString));
}

void ABlasterPlayerController::SetHudScore(const float score)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	
	if (!m_pHUD ||
	!m_pHUD->m_pCharacterOverlay ||
	!m_pHUD->m_pCharacterOverlay->ScoreAmount)
	{
		m_InitializeCharacterOverlay = true;
		m_Kills = score;
		return;
	}

	const FString scoreText = FString::Printf(TEXT("%d"), FMath::CeilToInt(score));
	m_pHUD->m_pCharacterOverlay->ScoreAmount->SetText(FText::FromString(scoreText));
}

void ABlasterPlayerController::SetHudDeaths(const float deaths)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	
	if (!m_pHUD ||
	!m_pHUD->m_pCharacterOverlay ||
	!m_pHUD->m_pCharacterOverlay->DeathsAmount)
	{
		m_InitializeCharacterOverlay = true;
		m_Deaths = deaths;
		return;
	}

	const FString deathsAmount = FString::Printf(TEXT("%d"), FMath::CeilToInt(deaths));
	m_pHUD->m_pCharacterOverlay->DeathsAmount->SetText(FText::FromString(deathsAmount));	
}

void ABlasterPlayerController::SetHudAmmo(const int ammo)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	
	if (!m_pHUD ||
	!m_pHUD->m_pCharacterOverlay ||
	!m_pHUD->m_pCharacterOverlay->AmmoCount) return;

	const FString ammoCount = FString::Printf(TEXT("%d"), ammo);
	m_pHUD->m_pCharacterOverlay->AmmoCount->SetText(FText::FromString(ammoCount));
	
}

void ABlasterPlayerController::SetHudCarriedAmmo(const int carriedAmmo)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	
	if (!m_pHUD ||
	!m_pHUD->m_pCharacterOverlay ||
	!m_pHUD->m_pCharacterOverlay->CarriedAmmoCount) return;

	const FString ammoCount = FString::Printf(TEXT("%d"), carriedAmmo);
	m_pHUD->m_pCharacterOverlay->CarriedAmmoCount->SetText(FText::FromString(ammoCount));
}

void ABlasterPlayerController::ShowAmmo(const bool showAmmo)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	
	if (!m_pHUD ||
	!m_pHUD->m_pCharacterOverlay ||
	!m_pHUD->m_pCharacterOverlay->AmmoContainer) return;

	m_pHUD->m_pCharacterOverlay->AmmoContainer->SetVisibility(showAmmo ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void ABlasterPlayerController::SetHudMatchCountDown(const float time)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	
	if (!m_pHUD ||
	!m_pHUD->m_pCharacterOverlay ||
	!m_pHUD->m_pCharacterOverlay->MatchCountDownText) return;

	const int minutes = FMath::FloorToInt(time / 60);
	const int seconds = FMath::FloorToInt(FMath::Fmod(time, 60));
	const FString timeFormatted = FString::Printf(TEXT("%02d:%02d"), minutes, seconds);
	m_pHUD->m_pCharacterOverlay->MatchCountDownText->SetText(FText::FromString(timeFormatted));
}

void ABlasterPlayerController::SetHudAnnouncementCountDown(const float time)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	
	if (!m_pHUD ||
	!m_pHUD->m_pAnnouncement ||
	!m_pHUD->m_pAnnouncement->AnnouncementTimer) return;

	const int minutes = FMath::FloorToInt(time / 60);
	const int seconds = FMath::FloorToInt(FMath::Fmod(time, 60));
	const FString timeFormatted = FString::Printf(TEXT("%02d:%02d"), minutes, seconds);
	m_pHUD->m_pAnnouncement->AnnouncementTimer->SetText(FText::FromString(timeFormatted));
}

float ABlasterPlayerController::GetServerTime() const
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds(); // If we are on the server we don't have a delay
	return GetWorld()->GetTimeSeconds() + m_ClientServerTimeDifference;
}

//Called on server
void ABlasterPlayerController::OnMatchStateSet(const FName state)
{
	m_MatchState = state;
	
	if (m_MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

//Called on client
void ABlasterPlayerController::OnRep_MatchState()
{
	if (m_MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* gameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode)
	{
		m_WarmUpDuration = gameMode->GetWarmUpDuration();
		m_MatchDuration = gameMode->GetGameDuration();
		m_LevelStartingTime = gameMode->GetLevelStartingTime();
		m_MatchState = gameMode->GetMatchState();

		ClientJoinedMidGame(m_MatchState, m_WarmUpDuration, m_MatchDuration, m_LevelStartingTime);

		if (m_pHUD && m_MatchState == MatchState::WaitingToStart) m_pHUD->AddAnnouncement();
	}
}

void ABlasterPlayerController::ClientJoinedMidGame_Implementation(FName matchState, float warmUpDuration, float gameDuration, float levelStartingTime)
{
	if (HasAuthority()) return; // This shouldn't be needed but it gets executed on the server for some reason.
	m_MatchState = matchState;
	m_WarmUpDuration = warmUpDuration;
	m_MatchDuration = gameDuration;
	m_LevelStartingTime = levelStartingTime;
	OnMatchStateSet(m_MatchState);
	
	if (m_pHUD && m_MatchState == MatchState::WaitingToStart) m_pHUD->AddAnnouncement();
}

void ABlasterPlayerController::HandleMatchHasStarted()
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	if (m_pHUD)
	{
		m_pHUD->AddCharacterOverlay();
		if (m_pHUD->m_pAnnouncement) m_pHUD->m_pAnnouncement->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABlasterPlayerController::SetHudTime()
{
	float timeLeft = 0.f;
	if (m_MatchState == MatchState::WaitingToStart) timeLeft = m_WarmUpDuration - GetServerTime() + m_LevelStartingTime;
	else if (m_MatchState == MatchState::InProgress) timeLeft = m_WarmUpDuration + m_MatchDuration - GetServerTime() + m_LevelStartingTime;
	
	const unsigned int secondsLeft = FMath::CeilToInt(timeLeft);
	if (secondsLeft != m_CountDownSeconds) // Only update once a full second has passed
	{
		if (m_MatchState == MatchState::WaitingToStart) SetHudAnnouncementCountDown(timeLeft);
		else if (m_MatchState == MatchState::InProgress) SetHudMatchCountDown(timeLeft);
	}
	
	m_CountDownSeconds = secondsLeft;
}

// Called on the client, executed on the server
void ABlasterPlayerController::ServerRequestServerTime_Implementation(float timeOfClientRequest)
{
	const float timeServerReceivedRequest = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(timeOfClientRequest, timeServerReceivedRequest);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float timeOfClientRequest,
	float timeServerReceivedRequest)
{
	// Calculate the round trip time
	const float roundTripTime = GetWorld()->GetTimeSeconds() - timeOfClientRequest;
	const float serverTime = timeServerReceivedRequest + roundTripTime * 0.5f;
	m_ClientServerTimeDifference = serverTime - GetWorld()->GetTimeSeconds();
}