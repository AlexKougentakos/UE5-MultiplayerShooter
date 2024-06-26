#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

	//Set up movement component
	m_pProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	m_pProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	checkf(m_pTracerEffect, TEXT("Tracer effect is nullptr"));

	m_pParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(
		m_pTracerEffect,
		m_pCollisionBox,
		FName(),
		GetActorLocation(),
		GetActorRotation(),
		EAttachLocation::KeepWorldPosition);
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

