#include "Shotgun.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Character/Components/LagCompensationComponent.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"
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

	APawn* pOwnerPawn = Cast<APawn>(GetOwner());
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
			if (pCharacter)
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
		//if (!pOwnerController && HasAuthority()) return;

		TArray<ABlasterCharacter*> hitCharacters{};
		for (const auto& pair : hitMap)
		{
			if (!pair.Key || !pOwnerController) continue;
			
			hitCharacters.Emplace(pair.Key);
			if (HasAuthority() && (!m_UseServerSideRewind || pOwnerPawn->IsLocallyControlled()))
			{
				UGameplayStatics::ApplyDamage(pair.Key, m_Damage * pair.Value, pOwnerController, this, UDamageType::StaticClass());
			}
		}

		if (!HasAuthority() && m_UseServerSideRewind) //Use server side rewind
		{
			m_pWeaponHolderController = m_pWeaponHolderController ? m_pWeaponHolderController : Cast<ABlasterPlayerController>(pOwnerController);
			m_pWeaponHolder = m_pWeaponHolder ? m_pWeaponHolder : Cast<ABlasterCharacter>(pOwnerPawn);

			if (!m_pWeaponHolder->IsLocallyControlled()) return;
			
			//This the amount of time that the server has to rewind back in order to get the character to his location
			//at the time that we shot
			const float time = m_pWeaponHolderController->GetServerTime() - m_pWeaponHolderController->GetSingleTripTime();
			m_pWeaponHolder->GetLagCompensationComponent()->ServerShotgunDamageRequest(hitCharacters, start, hitLocations, time, this);
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

