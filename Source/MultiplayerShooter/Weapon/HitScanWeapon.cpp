#include "HitScanWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
		FHitResult hitResult{};
		WeaponTraceHit(start, hitTarget, hitResult);
		
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
	}
	
}

void AHitScanWeapon::WeaponTraceHit(const FVector& traceStart, const FVector& hitTarget, FHitResult& outHitResult) const
{
	const FVector end = m_UseScatter ? GetVectorWithSpread(traceStart, hitTarget) : traceStart + (hitTarget - traceStart) * 1.2f;
	
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

FVector AHitScanWeapon::GetVectorWithSpread(const FVector& hitStart, const FVector& hitTarget) const
{
	const FVector vectorToTargetNormalized = (hitTarget - hitStart).GetSafeNormal();
	const FVector sphereCenter = hitStart + vectorToTargetNormalized * m_DistanceToSpreadSphere;
	const FVector randomVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, m_SphereRadius);
	const FVector end = sphereCenter + randomVector;
	const FVector toEndLocation = end - hitStart;

	// DrawDebugSphere(GetWorld(), sphereCenter, m_SphereRadius, 12, FColor::Red, true, 1.f);
	// DrawDebugSphere(GetWorld(), end, 5.f, 12, FColor::Green, true, 1.f);
	// DrawDebugLine(GetWorld(), hitStart, hitStart + toEndLocation * BULLET_TRACE_LENGTH, FColor::Green, true, 1.f);

	return {hitStart + toEndLocation * BULLET_TRACE_LENGTH / toEndLocation.Size()}; //Dividing to avoid overflow
}