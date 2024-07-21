#include "BlasterPlayerController.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/GameModes/BlasterGameMode.h"
#include "MultiplayerShooter/GameState/BlasterGameState.h"
#include "MultiplayerShooter/HUD/Announcement.h"
#include "MultiplayerShooter/HUD/BlasterHUD.h"
#include "MultiplayerShooter/HUD/CharacterOverlay.h"
#include "MultiplayerShooter/HUD/ReturnToMainMenu.h"
#include "MultiplayerShooter/PlayerState/BlasterPlayerState.h"
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

	ABlasterCharacter* pCharacter = Cast<ABlasterCharacter>(InPawn);
	if (!pCharacter) return;

	pCharacter->UpdateHudHealth();
	pCharacter->UpdateHudShield();
	pCharacter->UpdateHudAmmo();
	
}

void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHudTime();

	m_TimeSinceLastPingUpdate += DeltaSeconds;
	if (m_TimeSinceLastPingUpdate >= m_PingUpdateFrequency)
	{
		m_TimeSinceLastPingUpdate = 0.f;
		m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
		if (m_pHUD && m_pHUD->m_pCharacterOverlay && m_pHUD->m_pCharacterOverlay->PingText)
		{
			const FString pingString = FString::Printf(TEXT("Ping: %d"), FMath::CeilToInt(PlayerState->GetPingInMilliseconds()));
			m_pHUD->m_pCharacterOverlay->PingText->SetText(FText::FromString(pingString));
		}
	}
	HandleHighPingWarning(DeltaSeconds);
	HandleTimeSync(DeltaSeconds);
	PollInitialize();
}

void ABlasterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	checkf(InputComponent, TEXT("InputComponent is null"));
	InputComponent->BindAction("Quit", IE_Pressed, this, &ThisClass::ShowReturnToMainMenu);
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
			if (m_InitializeHealth)
			{
				SetHudHealth(m_Health, m_MaxHealth);
				m_InitializeHealth = false;
			}
			if (m_InitializeShield)
			{
				SetHudShield(m_Shield, m_MaxShield);
				m_InitializeShield = false;
			}
			if (m_InitializeKills)
			{
				SetHudScore(m_Kills);
				m_InitializeKills = false;
			}
			if (m_InitializeDeaths)
			{
				SetHudDeaths(m_Deaths);
				m_InitializeDeaths = false;
			}
			bool showAmmo = false;
			if (m_InitializeCarriedAmmo)
			{
				SetHudCarriedAmmo(m_CarriedAmmo);
				m_InitializeCarriedAmmo = false;
				showAmmo = true;
			}
			if (m_InitializeWeaponAmmo)
			{
				SetHudAmmo(m_WeaponAmmo);
				m_InitializeWeaponAmmo = false;
				showAmmo = true;
			}
			
			ShowAmmo(showAmmo);
		}
	}
}

void ABlasterPlayerController::HandleHighPingWarning(float DeltaSeconds)
{
	m_TimeSinceLastHighPingWarning += DeltaSeconds;
	if (m_TimeSinceLastHighPingWarning >= m_HighPingCheckFrequency || m_CheckPingFlag)
	{
		if (PlayerState->GetPingInMilliseconds() > m_HighPingThreshold)
		{
			HighPingWarning();
			m_CurrentHighPingAnimationElapsedTime = 0.f;
		}
		ServerReportPingStatus(PlayerState->GetPingInMilliseconds() > m_ServerSideRewindPingCap);
		m_TimeSinceLastHighPingWarning = 0.f;

		m_CheckPingFlag = false;
	}
	if (m_pCharacterOverlay && m_pCharacterOverlay->IsAnimationPlaying(m_pCharacterOverlay->PingWarningAnimation))
	{
		m_CurrentHighPingAnimationElapsedTime += DeltaSeconds;
		if (m_CurrentHighPingAnimationElapsedTime >= m_HighPingWarningDisplayTime)
		{
			StopHighPingWarning();
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
		m_InitializeHealth = true;
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

void ABlasterPlayerController::SetHudShield(const float shield, const float maxShield)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());

	if (!m_pHUD ||
		!m_pHUD->m_pCharacterOverlay ||
		!m_pHUD->m_pCharacterOverlay->ShieldBar ||
		!m_pHUD->m_pCharacterOverlay->ShieldText)
	{
		m_InitializeShield = true;
		m_Shield = shield;
		m_MaxShield = maxShield;
		return;
	}

	// Set shield bar
	const float shieldPercentage = shield / maxShield;
	m_pHUD->m_pCharacterOverlay->ShieldBar->SetPercent(shieldPercentage);

	// Set shield text
	const FString shieldString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(shield), FMath::CeilToInt(maxShield));
	m_pHUD->m_pCharacterOverlay->ShieldText->SetText(FText::FromString(shieldString));
}

void ABlasterPlayerController::SetHudScore(const float score)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	
	if (!m_pHUD ||
	!m_pHUD->m_pCharacterOverlay ||
	!m_pHUD->m_pCharacterOverlay->ScoreAmount)
	{
		m_InitializeKills = true;
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
		m_InitializeDeaths = true;
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
	!m_pHUD->m_pCharacterOverlay->AmmoCount)
	{
		m_InitializeWeaponAmmo = true;
		m_WeaponAmmo = ammo;
		return;
	}

	const FString ammoCount = FString::Printf(TEXT("%d"), ammo);
	m_pHUD->m_pCharacterOverlay->AmmoCount->SetText(FText::FromString(ammoCount));
	
}

void ABlasterPlayerController::SetHudCarriedAmmo(const int carriedAmmo)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	
	if (!m_pHUD ||
	!m_pHUD->m_pCharacterOverlay ||
	!m_pHUD->m_pCharacterOverlay->CarriedAmmoCount)
	{
		m_InitializeCarriedAmmo = true;
		m_CarriedAmmo = carriedAmmo;
		return;
	}

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

	if (time < 0.f && m_pHUD && m_pHUD->m_pAnnouncement && m_pHUD->m_pAnnouncement->AnnouncementTimer)
	{
		m_pHUD->m_pAnnouncement->AnnouncementTimer->SetText(FText::FromString("00:00"));
		return;
	}
	
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

	if (time < 0.f)
	{
		m_pHUD->m_pAnnouncement->AnnouncementTimer->SetText(FText::FromString("00:00"));
		return;
	}

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

void ABlasterPlayerController::ServerReportPingStatus_Implementation(bool highPing)
{
	OnHighPingWarning.Broadcast(highPing);
}

//Called on server
void ABlasterPlayerController::OnMatchStateSet(const FName state)
{
	m_MatchState = state;

	if (m_MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	if (m_MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::HighPingWarning()
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	if (m_pHUD && m_pHUD->m_pCharacterOverlay && m_MatchState == MatchState::InProgress)
	{
		UWidgetAnimation* pHighPingAnimation = m_pHUD->m_pCharacterOverlay->PingWarningAnimation;
		checkf(pHighPingAnimation, TEXT("Ping warning animation is null"));
		checkf(m_pHUD->m_pCharacterOverlay->PingWarning, TEXT("Ping warning widget is null"));
		
		m_pHUD->m_pCharacterOverlay->PingWarning->SetOpacity(1.f);
		if (m_pHUD->m_pCharacterOverlay->IsAnimationPlaying(pHighPingAnimation))
		{
			m_pHUD->m_pCharacterOverlay->StopAnimation(pHighPingAnimation);
		}
		m_pHUD->m_pCharacterOverlay->PlayAnimation(pHighPingAnimation, 0.f, 8);
	}
}

void ABlasterPlayerController::StopHighPingWarning()
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	if (m_pHUD && m_pHUD->m_pCharacterOverlay && m_pHUD->m_pCharacterOverlay->PingWarning)
	{
		const UWidgetAnimation* pHighPingAnimation = m_pHUD->m_pCharacterOverlay->PingWarningAnimation;
		checkf(pHighPingAnimation, TEXT("Ping warning animation is null"));
		
		m_pHUD->m_pCharacterOverlay->PingWarning->SetOpacity(0.f);
		if (m_pHUD->m_pCharacterOverlay->IsAnimationPlaying(pHighPingAnimation))
		{
			m_pHUD->m_pCharacterOverlay->StopAnimation(pHighPingAnimation);
		}
	}
}

void ABlasterPlayerController::ServerSendChatMessage_Implementation(const FString& Message)
{    
	if (!PlayerState) return;

	const FString SenderName = PlayerState->GetPlayerName();
    
	// Broadcast to all clients
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABlasterPlayerController* pPlayerController = Cast<ABlasterPlayerController>(*It))
		{
			pPlayerController->ClientReceiveChatMessage(SenderName, Message);
		}
	}
}

void ABlasterPlayerController::ClientReceiveChatMessage_Implementation(const FString& SenderName, const FString& Message)
{    
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	
	if (m_pHUD) m_pHUD->AddChatMessage(SenderName, Message);
}

//Called on client
void ABlasterPlayerController::OnRep_MatchState()
{
	if (m_MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	if (m_MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}


void ABlasterPlayerController::BroadcastElimination(APlayerState* pAttacker, APlayerState* pVictim,
                                                    AWeapon* pWeaponUsed)
{
	ClientEliminationAnnouncement(pAttacker, pVictim, pWeaponUsed);
}

void ABlasterPlayerController::ChatOpened()
{
	m_pHUD->ChatOpened();
}

void ABlasterPlayerController::SendChatMessage(const FString& Message)
{
	ServerSendChatMessage(Message);	
}

void ABlasterPlayerController::ClientEliminationAnnouncement_Implementation(APlayerState* pAttacker,
                                                                            APlayerState* pVictim, AWeapon* pWeaponUsed)
{
	APlayerState* self = GetPlayerState<APlayerState>();
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	if (!self || !pAttacker || !pAttacker || !pWeaponUsed || !m_pHUD) return;

	//We killed somebody else
	if (pAttacker == self && pVictim != self)
	{
		m_pHUD->AddEliminationAnnouncement(pVictim->GetPlayerName(), "You", pWeaponUsed);
		return;
	}
	//Somebody else killed us
	if (pVictim == self && pAttacker != self)
	{
		m_pHUD->AddEliminationAnnouncement("You", pAttacker->GetPlayerName(), pWeaponUsed);
		return;
	}
	if (pAttacker == self && pVictim == self)
	{
		m_pHUD->AddEliminationAnnouncement("Yourself", "You", pWeaponUsed);
		return;
	}

	m_pHUD->AddEliminationAnnouncement(pVictim->GetPlayerName(), pAttacker->GetPlayerName(), pWeaponUsed);
}

void ABlasterPlayerController::ShowReturnToMainMenu()
{
	checkf(m_ReturnToMainMenuWidgetClass, TEXT("Return to main menu widget class is null"));
	
	if (!m_pReturnToMainMenuWidget)
	{
		m_pReturnToMainMenuWidget = CreateWidget<UReturnToMainMenu>(this, m_ReturnToMainMenuWidgetClass);
	}

	m_IsReturnToMainMenuOpen = !m_IsReturnToMainMenuOpen;
	if (m_IsReturnToMainMenuOpen)
	{
		m_pReturnToMainMenuWidget->MenuSetup();
	}
	else
	{
		m_pReturnToMainMenuWidget->MenuTeardown();
	}
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	const ABlasterGameMode* gameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode)
	{
		m_WarmUpDuration = gameMode->GetWarmUpDuration();
		m_MatchDuration = gameMode->GetGameDuration();
		m_LevelStartingTime = gameMode->GetLevelStartingTime();
		m_CooldownDuration = gameMode->GetCooldownDuration();
		m_MatchState = gameMode->GetMatchState();

		ClientJoinedMidGame(m_MatchState, m_WarmUpDuration, m_MatchDuration, m_LevelStartingTime, m_CooldownDuration);

		if (m_pHUD && m_MatchState == MatchState::WaitingToStart) m_pHUD->AddAnnouncement();
	}
}

void ABlasterPlayerController::ClientJoinedMidGame_Implementation(FName matchState, float warmUpDuration, float gameDuration, float levelStartingTime, float cooldownDuration)
{
	if (HasAuthority()) return; // This shouldn't be needed but it gets executed on the server for some reason.
	m_MatchState = matchState;
	m_WarmUpDuration = warmUpDuration;
	m_MatchDuration = gameDuration;
	m_CooldownDuration = cooldownDuration;
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

void ABlasterPlayerController::HandleCooldown()
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	if (m_pHUD)
	{
		m_pHUD->m_pCharacterOverlay->RemoveFromParent();
		if (m_pHUD->m_pAnnouncement)
		{
			m_pHUD->m_pAnnouncement->SetVisibility(ESlateVisibility::Visible);

			const FString announcementTitle{"New Match Starts In: "};
			m_pHUD->m_pAnnouncement->AnnouncementTitle->SetText(FText::FromString(announcementTitle));
	
			const auto pGameState = Cast<ABlasterGameState>(GetWorld()->GetGameState());
			checkf(pGameState, TEXT("GameState is null"));

			const auto pPlayerState = GetPlayerState<ABlasterPlayerState>();
			checkf(pPlayerState, TEXT("PlayerState is null"));

			const TArray<ABlasterPlayerState*> topPlayers = pGameState->GetTopScoringPlayers();
			FString announcementDescription{};
			if (topPlayers.IsEmpty()) announcementDescription = "Nobody got a kill? Really?";
			else if (topPlayers.Num() == 1)
			{
				if (topPlayers[0] == pPlayerState) announcementDescription = "You are the top player!"; // For the local player
				else announcementDescription = FString::Printf(TEXT("%s is the top player!"), *topPlayers[0]->GetPlayerName());
			}
			else
			{
				announcementDescription = FString("Top Scoring Players: \n");
				for (const auto pTopPlayer : topPlayers)
					announcementDescription += FString::Printf(TEXT("%s\n"), *pTopPlayer->GetPlayerName());
			}

			m_pHUD->m_pAnnouncement->AnnouncementDescription->SetText(FText::FromString(announcementDescription));
		}
	}

	const auto pCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (pCharacter)
	{
		pCharacter->SetDisabledGameplay(true);
		pCharacter->GetCombatComponent()->FireButtonPressed(false); //Release the fire button when the game gets cut off
	}
}

void ABlasterPlayerController::SetHudTime()
{
	float timeLeft = 0.f;
	if (m_MatchState == MatchState::WaitingToStart) timeLeft = m_WarmUpDuration - GetServerTime() + m_LevelStartingTime;
	else if (m_MatchState == MatchState::InProgress) timeLeft = m_WarmUpDuration + m_MatchDuration - GetServerTime() + m_LevelStartingTime;
	else if (m_MatchState == MatchState::Cooldown) timeLeft = m_CooldownDuration + m_WarmUpDuration + m_MatchDuration - GetServerTime() + m_LevelStartingTime;
	unsigned int secondsLeft = FMath::CeilToInt(timeLeft);
	
	if (HasAuthority())
	{
		m_pGameMode = m_pGameMode ? m_pGameMode : Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));

		checkf(m_pGameMode, TEXT("Game mode is null"));
		secondsLeft = FMath::CeilToInt(m_pGameMode->GetCountdownTime());
	}
	
	if (secondsLeft != m_CountDownSeconds) // Only update once a full second has passed
	{
		if (m_MatchState == MatchState::WaitingToStart || m_MatchState == MatchState::Cooldown) SetHudAnnouncementCountDown(timeLeft);
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
	m_SingleTripTime = roundTripTime * 0.5f;
	const float serverTime = timeServerReceivedRequest + m_SingleTripTime;
	m_ClientServerTimeDifference = serverTime - GetWorld()->GetTimeSeconds();
}


