#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& hitTarget)
{
	Super::Fire(hitTarget);

	if (SpawnParticleEffect(hitTarget)) return;
}

bool AProjectileWeapon::SpawnParticleEffect(const FVector& hitTarget)
{
	// We only want to spawn the particle effect on the server
	if (!HasAuthority()) return true;
	
	const auto pWeaponMesh = GetWeaponMesh();
	const auto pMuzzleFlashSocket = pWeaponMesh->GetSocketByName(FName("MuzzleFlash"));

	//Various checks for points that should never be null
	checkf(pMuzzleFlashSocket, TEXT("MuzzleFlash socket is nullptr"));
	checkf(m_pProjectileClass, TEXT("Projectile class is nullptr"));

	//Set ownership and instigator to the player that owns the weapon
	FActorSpawnParameters spawnParameters{};
	spawnParameters.Owner = GetOwner();
	spawnParameters.Instigator = Cast<APawn>(GetOwner());

	
	const FTransform socketTransform = pMuzzleFlashSocket->GetSocketTransform(pWeaponMesh);
	const FVector vectorFromSocketToTarget = hitTarget - socketTransform.GetLocation();
	const FRotator rotationFromSocketToTarget = vectorFromSocketToTarget.Rotation(); // A vector's .Rotation() function gives us the rotation that the vector is pointing at

	if(m_pProjectileClass)
		GetWorld()->SpawnActor<AProjectile>(m_pProjectileClass, socketTransform.GetLocation(), rotationFromSocketToTarget, spawnParameters);
	return true;
}
