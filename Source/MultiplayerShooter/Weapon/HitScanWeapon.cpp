#include "HitScanWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/Character/Components/LagCompensationComponent.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

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
    bool isHeadShot{false};
    if (!muzzleSocket) return;
    
    const FTransform muzzleTransform = muzzleSocket->GetSocketTransform(GetWeaponMesh());
    const FVector start = muzzleTransform.GetLocation();
    FHitResult hitResult{};
    WeaponTraceHit(start, hitTarget, hitResult);
    if (!hitResult.bBlockingHit) return;
    
    ABlasterCharacter* pCharacter = Cast<ABlasterCharacter>(hitResult.GetActor());
    if (pCharacter)
    {
       if (HasAuthority() && (!m_UseServerSideRewind || pOwnerPawn->IsLocallyControlled()))
       {
          float damage = m_Damage;
          auto damageType = UDamageType::StaticClass();
          if (hitResult.BoneName.ToString() == "head")
          {
             isHeadShot = true;
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
    
    if (isHeadShot && m_pHeadshotImpactSound)
       UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pHeadshotImpactSound, hitResult.ImpactPoint);
    else if (m_pImpactSound)
       UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pImpactSound, hitResult.ImpactPoint);
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

UParticleSystem* AHitScanWeapon::GetImpactEffect(const UPhysicalMaterial* physicalMaterial) const
{
	if (physicalMaterial == m_pGrassPhysicalMaterial) return m_pGrassImpactEffect;
	if (physicalMaterial == m_pWoodPhysicalMaterial) return m_pWoodImpactEffect;
	if (physicalMaterial == m_pMetalPhysicalMaterial) return m_pMetalImpactEffect;
	if (physicalMaterial == m_pPlayerPhysicalMaterial) return m_pPlayerImpactEffect;
	if (physicalMaterial == m_pRockPhysicalMaterial) return m_pRockImpactEffect;
	return m_pMetalImpactEffect;
}

const UPhysicalMaterial* AHitScanWeapon::GetMaterialOfActor(AActor* OtherActor) const
{
	const UPhysicalMaterial* materialOfHitObject = {};
	const ABlasterCharacter* pBlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (pBlasterCharacter)
	{
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