#include "Shotgun.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Character/Components/LagCompensationComponent.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

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
		TMap<ABlasterCharacter*, int> headshotHitMap{};
		for(const auto& hitLocation : hitLocations)
		{
			FHitResult hitResult{};
			WeaponTraceHit(start, hitLocation, hitResult);
			bool playSoundEffect = true;

			ABlasterCharacter* pCharacter = Cast<ABlasterCharacter>(hitResult.GetActor());
			if (pCharacter)
			{
				const bool isHeadshot = hitResult.BoneName == "head";

				if (isHeadshot)
				{
					if (headshotHitMap.Contains(pCharacter)) headshotHitMap[pCharacter]++;
					else headshotHitMap.Emplace(pCharacter, 1);

					if (m_pHeadshotImpactSound)
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pHeadshotImpactSound, hitResult.ImpactPoint);
				}
				else
				{
					if (hitMap.Contains(pCharacter)) hitMap[pCharacter]++;
					else hitMap.Emplace(pCharacter, 1);

					if (m_pPlayerImpactEffect)
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pPlayerImpactSound, hitResult.ImpactPoint);
				}
			}

			UParticleSystem* pEffect = GetImpactEffect(GetMaterialOfActor(hitResult.GetActor()));
			//Calculate the correct rotation for the impact effect
			if (pEffect)
			{
				FVector upVector = hitResult.ImpactNormal;
				FVector forwardVector = FVector::CrossProduct(upVector, FVector::RightVector);
				if (forwardVector.IsNearlyZero())
				{
					forwardVector = FVector::CrossProduct(upVector, FVector::ForwardVector);
				}
				FVector rightVector = FVector::CrossProduct(upVector, forwardVector);
				FRotator newRotation = UKismetMathLibrary::MakeRotationFromAxes(forwardVector, rightVector, upVector);

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pEffect, hitResult.ImpactPoint, newRotation);
			}
			
			if (playSoundEffect)
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pImpactSound, hitResult.ImpactPoint);
		}
		
		//Apply damage
		//Instigator controllers are null on simulated proxies, we are only using this for the damage application on the server side
		// so we only care if it's null on the server
		//if (!pOwnerController && HasAuthority()) return;

		TArray<ABlasterCharacter*> hitCharacters{};
		TMap<ABlasterCharacter*, float> damageMap{};
		//Get the total damage for body shots for each actor
		for (const auto& pair : hitMap)
		{
			if (!pair.Key) continue;

			damageMap.Emplace(pair.Key, pair.Value * m_Damage);
			hitCharacters.AddUnique(pair.Key);
		}
		//Get the total damage for headshots for each actor
		for (const auto& pair : headshotHitMap)
		{
			if (!pair.Key) continue;

			//Add the headshot damage to the total damage
			if (damageMap.Contains(pair.Key))
			{
				damageMap[pair.Key] += pair.Value * m_HeadShotDamage;
			}
			//If it doesn't exist, create a new entry
			else
			{
				damageMap.Emplace(pair.Key, pair.Value * m_HeadShotDamage);
			}
			
			hitCharacters.AddUnique(pair.Key);
		}

		//Apply the damage
		for (const auto& pair : damageMap)
		{
			if (!pair.Key || !pOwnerController) continue;

			if (HasAuthority() && (!m_UseServerSideRewind || pOwnerPawn->IsLocallyControlled()))
			{
				UGameplayStatics::ApplyDamage(pair.Key, pair.Value, pOwnerController, this, UDamageType::StaticClass());
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

void AShotgun::ShotgunGetVectorWithSpread(const FVector& hitTarget, TArray<FVector_NetQuantize>& outShotLocations, float totalSpreadMultiplier) const
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

