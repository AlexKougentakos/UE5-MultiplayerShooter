#include "ProjectileRocket.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "RocketMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AProjectileRocket::AProjectileRocket()
{
	m_pProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	m_pProjectileMesh->SetupAttachment(RootComponent);
	m_pProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	m_pRocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));
	m_pRocketMovementComponent->bRotationFollowsVelocity = true;
	m_pRocketMovementComponent->SetIsReplicated(true);
}


void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority())
	{
		// ReSharper disable once CppBoundToDelegateMethodIsNotMarkedAsUFunction
		// It's an inherited function which defines UFunction signature
		m_pCollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}

	SpawnTrailSystem(); //For the following smoke & fire trail
	
	// Handle the sound
	checkf(m_pProjectileInAirLoop, TEXT("No in air loop sound set for rocket"));
	checkf(m_pProjectileInAirLoopAttenuation, TEXT("No in air loop sound attenuation set for rocket"));

	m_pProjectileInAirLoopComponent = UGameplayStatics::SpawnSoundAttached(
		m_pProjectileInAirLoop,
		GetRootComponent(),
		FName(),
		GetActorLocation(),
		EAttachLocation::KeepWorldPosition,
		false,
		1.0f,
		1.0f,
		0.0f,
		m_pProjectileInAirLoopAttenuation,
		nullptr,
		false);
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner()) return;
	
	DoDamageWithFallOff(m_InnerDamageRadius, m_OuterDamageRadius, m_Damage);
	StartDestroyTimer();

	//Play effect
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GetImpactEffect(GetMaterialOfActor(OtherActor)), GetActorTransform());

	//Play sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pImpactSound, GetActorLocation());

	// Handle the projectile mesh
	checkf(m_pProjectileMesh, TEXT("Rocket mesh is nullptr"));
	m_pProjectileMesh->SetVisibility(false);
	checkf(m_pCollisionBox, TEXT("Collision box is nullptr"));
	m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	checkf(m_pTrailEffectComponent, TEXT("Trail effect component is nullptr"));
	m_pTrailEffectComponent->Deactivate();

	//Handle the sound
	checkf(m_pProjectileInAirLoopComponent, TEXT("Projectile in air loop component is nullptr"));
	m_pProjectileInAirLoopComponent->Stop();	
}
