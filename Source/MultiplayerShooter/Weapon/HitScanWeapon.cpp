#include "HitScanWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "Particles/ParticleSystemComponent.h"

void AHitScanWeapon::Fire(const FVector& hitTarget)
{
	Super::Fire(hitTarget);

	const APawn* pOwnerPawn = Cast<APawn>(GetOwner());
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
		const FVector end = start + (hitTarget - start) * 1.2f;

		FHitResult hitResult{};
		UWorld* pWorld = GetWorld();

		checkf(pWorld, TEXT("World is nullptr"));
		pWorld->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility);

		const FVector beamEnd = hitResult.bBlockingHit ? hitResult.ImpactPoint : end;
		
		if (hitResult.bBlockingHit)
		{
			ABlasterCharacter* pCharacter = Cast<ABlasterCharacter>(hitResult.GetActor());
			if (pCharacter && HasAuthority())
			{
				UGameplayStatics::ApplyDamage(pCharacter, m_Damage, pOwnerController, this, UDamageType::StaticClass());
			}

			if (m_pImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_pImpactParticles, hitResult.ImpactPoint, hitResult.ImpactNormal.Rotation());
			}
		}

		if (m_pBeamParticles)
		{
			UParticleSystemComponent* pBeam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_pBeamParticles, muzzleTransform);
			checkf(pBeam, TEXT("Beam is nullptr"));
				pBeam->SetVectorParameter("Target", beamEnd);
		}
	}
	
}
