#include "ShieldPickup.h"

#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Character/Components/BuffComponent.h"

void AShieldPickup::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
                                    UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	Super::OnSphereOverlap(overlappedComponent, otherActor, otherComponent, otherBodyIndex, bFromSweep, sweepResult);

	const ABlasterCharacter* pPlayer = Cast<ABlasterCharacter>(otherActor);
	if (!pPlayer) return;

	UBuffComponent* pBuffComponent = pPlayer->GetBuffComponent();
	if (!pBuffComponent) return;
	
	pBuffComponent->AddShield(m_ShieldAmount, m_ShieldingTime);
	
	Destroy();
}
