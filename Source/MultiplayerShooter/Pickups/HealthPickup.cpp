﻿#include "HealthPickup.h"

#include "NiagaraFunctionLibrary.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Character/Components/BuffComponent.h"


AHealthPickup::AHealthPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
                                    UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	Super::OnSphereOverlap(overlappedComponent, otherActor, otherComponent, otherBodyIndex, bFromSweep, sweepResult);

	const ABlasterCharacter* pPlayer = Cast<ABlasterCharacter>(otherActor);
	if (!pPlayer) return;

	UBuffComponent* pBuffComponent = pPlayer->GetBuffComponent();
	if (!pBuffComponent) return;
	
	pBuffComponent->Heal(m_HealAmount, m_HealingTime);
	

	Destroy();
}

