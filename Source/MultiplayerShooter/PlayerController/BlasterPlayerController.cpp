#include "BlasterPlayerController.h"

#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/HUD/BlasterHUD.h"
#include "MultiplayerShooter/HUD/CharacterOverlay.h"

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController()) ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	
}

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	m_pHUD = Cast<ABlasterHUD>(GetHUD());
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

void ABlasterPlayerController::SetHudHealth(const float health, const float maxHealth)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());

	if (!m_pHUD ||
		!m_pHUD->m_pCharacterOverlay ||
		!m_pHUD->m_pCharacterOverlay->HealthBar ||
		!m_pHUD->m_pCharacterOverlay->HealthText) return;

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
	!m_pHUD->m_pCharacterOverlay->ScoreAmount) return;

	const FString scoreText = FString::Printf(TEXT("%d"), FMath::CeilToInt(score));
	m_pHUD->m_pCharacterOverlay->ScoreAmount->SetText(FText::FromString(scoreText));
}

void ABlasterPlayerController::SetHudDeaths(const float deaths)
{
	m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());
	
	if (!m_pHUD ||
	!m_pHUD->m_pCharacterOverlay ||
	!m_pHUD->m_pCharacterOverlay->DeathsAmount) return;

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

float ABlasterPlayerController::GetServerTime() const
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds(); // If we are on the server we don't have a delay
	return GetWorld()->GetTimeSeconds() + m_ClientServerTimeDifference;
}

void ABlasterPlayerController::SetHudTime()
{
	const unsigned int secondsLeft = FMath::CeilToInt(m_MatchCountDown - GetServerTime());
	if (secondsLeft != m_CountDownSeconds) // Only update once a full second has passed
	{
		SetHudMatchCountDown(secondsLeft);
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