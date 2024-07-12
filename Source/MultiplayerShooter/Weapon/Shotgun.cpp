#include "Shotgun.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "Particles/ParticleSystemComponent.h"

AShotgun::AShotgun()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShotgun::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& hitLocations)
{
	AWeapon::Fire(FVector{});

	const APawn* pOwnerPawn = Cast<APawn>(GetOwner());
	if (!pOwnerPawn) return;
	AController* pOwnerController = pOwnerPawn->GetController();

	const USkeletalMeshSocket* muzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (muzzleSocket)
	{
		const FTransform muzzleTransform = muzzleSocket->GetSocketTransform(GetWeaponMesh());
		const FVector start = muzzleTransform.GetLocation();

		//Maps each player that got hit with the amount of times they got hit
		TMap<ABlasterCharacter*, int> hitMap{};
		for(const auto& hitLocation : hitLocations)
		{
			FHitResult hitResult{};
			WeaponTraceHit(start, hitLocation, hitResult);

			ABlasterCharacter* pCharacter = Cast<ABlasterCharacter>(hitResult.GetActor());
			if (pCharacter && HasAuthority())
			{
				if (hitMap.Contains(pCharacter))
				{
					hitMap[pCharacter]++;
				}
				else
				{
					hitMap.Emplace(pCharacter, 1);
				}
			}

			if (m_pImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_pImpactParticles, hitResult.ImpactPoint, hitResult.ImpactNormal.Rotation());
			}
		}
		
		//Apply damage
		//Instigator controllers are null on simulated proxies, we are only using this for the damage application on the server side
		// so we only care if it's null on the server
		if (!pOwnerController && HasAuthority()) return;
		for (const auto& pair : hitMap)
		{
			if (HasAuthority() && pOwnerController)
				UGameplayStatics::ApplyDamage(pair.Key, m_Damage * pair.Value, pOwnerController, this, UDamageType::StaticClass());
		}
	}
}

void AShotgun::ShotgunGetVectorWithSpread(const FVector& hitTarget, TArray<FVector_NetQuantize>& outShotLocations) const
{
	const USkeletalMeshSocket* muzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (!muzzleSocket) return;
	
	const FTransform muzzleTransform = muzzleSocket->GetSocketTransform(GetWeaponMesh());
	const FVector start = muzzleTransform.GetLocation();
	const FVector vectorToTargetNormalized = (hitTarget - start).GetSafeNormal();
	const FVector sphereCenter = start + vectorToTargetNormalized * m_DistanceToSpreadSphere;

	for (int i = 0; i < m_NumberOfPellets; ++i)
	{
		const FVector randomVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, m_SphereRadius);
		const FVector end = sphereCenter + randomVector;
		const FVector toEndLocation = end - start;
		const FVector returnVector = start + toEndLocation * BULLET_TRACE_LENGTH / toEndLocation.Size(); //Dividing to avoid overflow
		outShotLocations.Add(returnVector);
	}
}

