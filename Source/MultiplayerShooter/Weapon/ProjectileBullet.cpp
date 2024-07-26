// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Character/Components/LagCompensationComponent.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"

AProjectileBullet::AProjectileBullet()
{
	//Set up movement component
	m_pProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	m_pProjectileMovementComponent->bRotationFollowsVelocity = true;
	m_pProjectileMovementComponent->SetIsReplicated(true);
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	const ABlasterCharacter* pOwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
	if (!pOwnerCharacter) return;

	ABlasterPlayerController* pOwnerController = Cast<ABlasterPlayerController>(pOwnerCharacter->GetController());
	if (!pOwnerController) return;

	if (!OtherActor) return;
	
	if (pOwnerCharacter->HasAuthority() && !m_UseServerSideRewind)
	{
		const float damage = Hit.BoneName.ToString() == "head" ? m_HeadShotDamage : m_Damage;
		UGameplayStatics::ApplyDamage(OtherActor, damage, pOwnerController, this, UDamageType::StaticClass());

		//Super should be called last since we are calling the Destroy function in it
		OnHeadShot(OtherActor);
		return;
	}

	ABlasterCharacter* pHitCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (m_UseServerSideRewind && pOwnerCharacter->IsLocallyControlled() && pHitCharacter)
	{
		pOwnerCharacter->GetLagCompensationComponent()->ServerProjectileDamageRequest(
			pHitCharacter,
			m_SpawnLocation,
			m_InitialVelocity,
			pOwnerController->GetServerTime() - pOwnerController->GetSingleTripTime(),
			pOwnerCharacter->GetEquippedWeapon());
	}
	
	//Super should be called last since we are calling the Destroy function in it
	Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
}
