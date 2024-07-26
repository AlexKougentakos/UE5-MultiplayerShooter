#include "Projectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
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

void AProjectile::Destroyed()
{
	Super::Destroyed();
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor) return;
	
	if (HasAuthority())
	{
		MulticastOnHit(GetMaterialOfActor(OtherActor), false);
	}
}

void AProjectile::OnHeadShot(AActor* pHitActor)
{
	if (!pHitActor) return;
	
	if (HasAuthority())
	{
		MulticastOnHit(GetMaterialOfActor(pHitActor), true);
	}
}

void AProjectile::MulticastOnHit_Implementation(const UPhysicalMaterial* physicalMaterial, bool isHeadShot)
{
	checkf(m_pMetalImpactEffect, TEXT("Impact effect is nullptr"));
	checkf(m_pImpactSound, TEXT("Impact sound is nullptr"));

	//Play effect
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GetImpactEffect(physicalMaterial), GetActorTransform());

	
	//Play sound
	if (isHeadShot)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pHeadshotImpactSound, GetActorLocation());
	else if (physicalMaterial == m_pPlayerPhysicalMaterial)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pPlayerImpactSound, GetActorLocation());
	else
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pImpactSound, GetActorLocation());

	Destroy();
}


const UPhysicalMaterial* AProjectile::GetMaterialOfActor(AActor* OtherActor) const
{
	const UPhysicalMaterial* materialOfHitObject = {};
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

	return materialOfHitObject;
}



void AProjectile::SpawnTrailSystem()
{
		
	checkf(m_pTrailEffect, TEXT("No trail effect set for rocket"));

	m_pTrailEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(m_pTrailEffect,
		GetRootComponent(),
		FName(),
		GetActorLocation(),
		GetActorRotation(),
		EAttachLocation::KeepWorldPosition,
		false);

}

void AProjectile::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(m_DestructionTimer, this, &AProjectile::DestroyTimerFinished, m_DestroyTime, false);
}

void AProjectile::DestroyTimerFinished()
{
	Destroy();
}

void AProjectile::DoDamageWithFallOff(const float innerDamageRadius, const float outerDamageRadius, const float damageAmount)
{
	//This is set in the ProjectileWeapon.cpp when firing the projectile
	const APawn* pFiringPawn = GetInstigator();
	if(pFiringPawn && HasAuthority()) //Only apply damage on the server
	{
		AController* pFiringPawnController = pFiringPawn->GetController();
		if (pFiringPawnController) 
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
			   damageAmount
			   ,damageAmount * 0.33f
			   ,GetActorLocation(),
			   innerDamageRadius,
			   outerDamageRadius,
			   1.0f,
			   UDamageType::StaticClass(),
			   TArray<AActor*>(),
			   this,
			   pFiringPawnController);
		}
	}
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
