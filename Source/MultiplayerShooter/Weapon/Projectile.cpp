#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/MultiplayerShooter.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "Sound/SoundCue.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	m_pCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(m_pCollisionBox);

	//Set up collisions
	m_pCollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	m_pCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	m_pCollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	m_pCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	m_pCollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);

	//Set up movement component
	m_pProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	m_pProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	
	if (m_pTracerEffect)
	{
		m_pParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(
			m_pTracerEffect,
			m_pCollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition);
	}
	
	if (!HasAuthority()) return;
	m_pCollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor) return;
	
	if (HasAuthority())
	{
		const UPhysicalMaterial* materialOfHitObject{};
		const ABlasterCharacter* pBlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
		if (pBlasterCharacter)
		{
			//BlasterCharacter->MulticastHit();
			materialOfHitObject = m_pPlayerPhysicalMaterial;
		}
		else
		{
			bool validMaterial = false;

			if (const auto pMesh = OtherActor->FindComponentByClass<UStaticMeshComponent>())
				if (const auto pBodyInstance = pMesh->GetBodyInstance())
					if (pBodyInstance->GetSimplePhysicalMaterial())
						validMaterial = true;

			if (!validMaterial) 
				materialOfHitObject = m_pMetalPhysicalMaterial;
			else materialOfHitObject = OtherActor->FindComponentByClass<UStaticMeshComponent>()->GetBodyInstance()->GetSimplePhysicalMaterial();	
		}

		MulticastOnHit(materialOfHitObject);
	}
}

void AProjectile::MulticastOnHit_Implementation(const UPhysicalMaterial* physicalMaterial)
{
	checkf(m_pMetalImpactEffect, TEXT("Impact effect is nullptr"));
	checkf(m_pImpactSound, TEXT("Impact sound is nullptr"));

	//Play effect
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GetImpactEffect(physicalMaterial), GetActorTransform());

	//Play sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pImpactSound, GetActorLocation());

	Destroy();
}

UParticleSystem* AProjectile::GetImpactEffect(const UPhysicalMaterial* physicalMaterial) const
{
	if (physicalMaterial == m_pGrassPhysicalMaterial) return m_pGrassImpactEffect;
	if (physicalMaterial == m_pWoodPhysicalMaterial) return m_pWoodImpactEffect;
	if (physicalMaterial == m_pMetalPhysicalMaterial) return m_pMetalImpactEffect;
	if (physicalMaterial == m_pPlayerPhysicalMaterial) return m_pPlayerImpactEffect;
	if (physicalMaterial == m_pRockPhysicalMaterial) return m_pRockImpactEffect;
	return m_pMetalImpactEffect;
}
