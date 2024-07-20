#include "HitScanWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Character/Components/LagCompensationComponent.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"
#include "Particles/ParticleSystemComponent.h"

void AHitScanWeapon::Fire(const FVector& hitTarget)
{
	Super::Fire(hitTarget);

	APawn* pOwnerPawn = Cast<APawn>(GetOwner());
	if (!pOwnerPawn) return;
	AController* pOwnerController = pOwnerPawn->GetController();
	//Instigator controllers are null on simulated proxies, we are only using this for the damage application on the server side
	// so we only care if it's null on the server
	if (!pOwnerController && HasAuthority()) return; 

	const USkeletalMeshSocket* muzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (muzzleSocket)
	{
		const FTransform muzzleTransform = muzzleSocket->GetSocketTransform(GetWeaponMesh());
		const FVector start = muzzleTransform.GetLocation();
		FHitResult hitResult{};
		WeaponTraceHit(start, hitTarget, hitResult);
		
		if (hitResult.bBlockingHit)
		{
			ABlasterCharacter* pCharacter = Cast<ABlasterCharacter>(hitResult.GetActor());
			if (pCharacter)
			{
				if (HasAuthority() && (!m_UseServerSideRewind || pOwnerPawn->IsLocallyControlled()))
				{
					float damage = m_Damage;
					auto damageType = UDamageType::StaticClass();
					if (hitResult.BoneName.ToString() == "head")
					{
						damage = m_HeadShotDamage;
						damageType = UHeadshotDamageType::StaticClass();
					}
					UGameplayStatics::ApplyDamage(pCharacter, damage, pOwnerController, this, damageType);
				}
				if (!HasAuthority() && m_UseServerSideRewind)
				{
					m_pWeaponHolderController = m_pWeaponHolderController ? m_pWeaponHolderController : Cast<ABlasterPlayerController>(pOwnerController);
					m_pWeaponHolder = m_pWeaponHolder ? m_pWeaponHolder : Cast<ABlasterCharacter>(pOwnerPawn);

					if (!m_pWeaponHolder->IsLocallyControlled()) return;
					
					//This the amount of time that the server has to rewind back in order to get the character to his location
					//at the time that we shot
					const float time = m_pWeaponHolderController->GetServerTime() - m_pWeaponHolderController->GetSingleTripTime();
					m_pWeaponHolder->GetLagCompensationComponent()->ServerDamageRequest(pCharacter, start, hitTarget, time, this);
				}
			}

			if (m_pImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_pImpactParticles, hitResult.ImpactPoint, hitResult.ImpactNormal.Rotation());
			}
		}
	}
	
}

void AHitScanWeapon::WeaponTraceHit(const FVector& traceStart, const FVector& hitTarget, FHitResult& outHitResult) const
{
	const FVector end = traceStart + (hitTarget - traceStart) * 1.2f;
	
	const UWorld* pWorld = GetWorld();
	checkf(pWorld, TEXT("World is nullptr"));
	
	pWorld->LineTraceSingleByChannel(outHitResult, traceStart, end, ECollisionChannel::ECC_Visibility);

	const FVector beamEnd = outHitResult.bBlockingHit ? outHitResult.ImpactPoint : end;

	if (m_pBeamParticles)
	{
		UParticleSystemComponent* pBeam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_pBeamParticles, traceStart, FRotator::ZeroRotator, true);
		checkf(pBeam, TEXT("Beam is nullptr"));
		pBeam->SetVectorParameter("Target", beamEnd);
	}
}