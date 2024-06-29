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

	checkf(m_pTracerEffect, TEXT("Tracer effect is nullptr"));

	m_pParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(
		m_pTracerEffect,
		m_pCollisionBox,
		FName(),
		GetActorLocation(),
		GetActorRotation(),
		EAttachLocation::KeepWorldPosition);

	if (!HasAuthority()) return;
	m_pCollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::Destroyed()
{
	Super::Destroyed();
	
	checkf(m_pImpactEffect, TEXT("Impact effect is nullptr"));
	checkf(m_pImpactSound, TEXT("Impact sound is nullptr"));

	//Play effect
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_pImpactEffect, GetActorTransform());

	//Play sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pImpactSound, GetActorLocation());
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	//Temp: to be replaced in the inherited classes
	ABlasterCharacter* pBlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (pBlasterCharacter) pBlasterCharacter->MulticastHit();
	
	Destroy();
}

