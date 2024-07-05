#include "ProjectileRocket.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AProjectileRocket::AProjectileRocket()
{
	m_pRocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	m_pRocketMesh->SetupAttachment(RootComponent);
	m_pRocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	
	checkf(m_pTrailEffect, TEXT("No trail effect set for rocket"));

	m_pTrailEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(m_pTrailEffect,
		GetRootComponent(),
		FName(),
		GetActorLocation(),
		GetActorRotation(),
		EAttachLocation::KeepWorldPosition,
		false);

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

void AProjectileRocket::DestroyTimerFinished()
{
	Destroy();
}


void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//This is set in the ProjectileWeapon.cpp when firing the projectile
	const APawn* pFiringPawn = GetInstigator();
	if(pFiringPawn && HasAuthority()) //Only apply damage on the server
	{
		AController* pFiringPawnController = pFiringPawn->GetController();
		if (pFiringPawnController) 
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
			   m_Damage
			   ,m_Damage * 0.33f
			   ,GetActorLocation(),
			   200.0f,
			   500.0f,
			   1.0f,
			   UDamageType::StaticClass(),
			   TArray<AActor*>(),
			   this,
			   pFiringPawnController);
		}
	}
	
	GetWorldTimerManager().SetTimer(m_DestructionTimer, this, &AProjectileRocket::DestroyTimerFinished, m_DestroyTime, false);

	//Play effect
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GetImpactEffect(GetMaterialOfActor(OtherActor)), GetActorTransform());

	//Play sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pImpactSound, GetActorLocation());

	// Handle the projectile mesh
	checkf(m_pRocketMesh, TEXT("Rocket mesh is nullptr"));
	m_pRocketMesh->SetVisibility(false);
	checkf(m_pCollisionBox, TEXT("Collision box is nullptr"));
	m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	checkf(m_pTrailEffectComponent, TEXT("Trail effect component is nullptr"));
	m_pTrailEffectComponent->Deactivate();

	//Handle the sound
	checkf(m_pProjectileInAirLoopComponent, TEXT("Projectile in air loop component is nullptr"));
	m_pProjectileInAirLoopComponent->Stop();	
}
