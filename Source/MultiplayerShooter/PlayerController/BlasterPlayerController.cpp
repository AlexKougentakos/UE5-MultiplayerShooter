#include "BlasterPlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/HUD/BlasterHUD.h"
#include "MultiplayerShooter/HUD/CharacterOverlay.h"

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

void ABlasterPlayerController::SetHudHealth(float health, float maxHealth)
{
	//m_pHUD = m_pHUD ? m_pHUD : Cast<ABlasterHUD>(GetHUD());

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
