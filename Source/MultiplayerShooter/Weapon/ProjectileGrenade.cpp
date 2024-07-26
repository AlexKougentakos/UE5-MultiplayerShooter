#include "ProjectileGrenade.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AProjectileGrenade::AProjectileGrenade()
{
	//Set up mesh
	m_pProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade Mesh"));
	m_pProjectileMesh->SetupAttachment(RootComponent);
	m_pProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Set up movement component
	m_pProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	m_pProjectileMovementComponent->bRotationFollowsVelocity = true;
	m_pProjectileMovementComponent->SetIsReplicated(true);
	m_pProjectileMovementComponent->bShouldBounce = true;
}

void AProjectileGrenade::BeginPlay()
{
	AActor::BeginPlay();

	StartDestroyTimer();
	SpawnTrailSystem();
	
	m_pProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectileGrenade::OnBounce);
}

void AProjectileGrenade::OnBounce(const FHitResult& HitResult, const FVector& ImpactVelocity)
{
	checkf(m_pBounceSound, TEXT("Bounce sound is nullptr"));

	const float impactStrengthMultiplier = ImpactVelocity.Size() / m_pProjectileMovementComponent->InitialSpeed;
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pBounceSound, GetActorLocation(), impactStrengthMultiplier);
}

void AProjectileGrenade::Destroyed()
{
	DoDamageWithFallOff(m_InnerDamageRadius, m_OuterDamageRadius, m_Damage);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_pMetalImpactEffect, GetActorLocation(), FRotator::ZeroRotator, true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pImpactSound, GetActorLocation());
	Super::Destroyed();
}
