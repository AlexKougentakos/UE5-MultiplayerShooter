#include "HitScanWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"

void AHitScanWeapon::Fire(const FVector& hitTarget)
{
	Super::Fire(hitTarget);

	const APawn* pOwnerPawn = Cast<APawn>(GetOwner());
	if (!pOwnerPawn) return;
	AController* pOwnerController = pOwnerPawn->GetController();
	if (!pOwnerController) return;

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
