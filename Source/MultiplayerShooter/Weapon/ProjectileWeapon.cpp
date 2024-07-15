#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/ProjectileMovementComponent.h"

void AProjectileWeapon::Fire(const FVector& hitTarget)
{
	Super::Fire(hitTarget);

	SpawnParticleEffect(hitTarget);
}

void AProjectileWeapon::SpawnParticleEffect(const FVector& hitTarget)
{	
	const auto pWeaponMesh = GetWeaponMesh();
	const auto pMuzzleFlashSocket = pWeaponMesh->GetSocketByName(FName("MuzzleFlash"));

	//Various checks for points that should never be null
	checkf(pMuzzleFlashSocket, TEXT("MuzzleFlash socket is nullptr"));
	checkf(m_pProjectileClass, TEXT("Projectile class is nullptr"));

	const auto pInstigatorPawn = Cast<APawn>(GetOwner());
	if (!pInstigatorPawn) return;
	
	//Set ownership and instigator to the player that owns the weapon
	FActorSpawnParameters spawnParameters{};
	spawnParameters.Owner = GetOwner();
	spawnParameters.Instigator = pInstigatorPawn;

	
	const FTransform socketTransform = pMuzzleFlashSocket->GetSocketTransform(pWeaponMesh);
	const FVector vectorFromSocketToTarget = hitTarget - socketTransform.GetLocation();
	const FRotator rotationFromSocketToTarget = vectorFromSocketToTarget.Rotation(); // A vector's .Rotation() function gives us the rotation that the vector is pointing at

	AProjectile* pSpawnedActor{};
	if (m_UseServerSideRewind)
	{
		if (pInstigatorPawn->HasAuthority())
		{
			if (pInstigatorPawn->IsLocallyControlled()) //server, host - use replicated projectile, no need for server side rewind
			{
				pSpawnedActor = GetWorld()->SpawnActor<AProjectile>(m_pProjectileClass, socketTransform.GetLocation(), rotationFromSocketToTarget, spawnParameters);
				pSpawnedActor->SetServerSideRewind(false);
				pSpawnedActor->SetDamage(m_Damage);
			}
			else //spawned on server, not locally controlled , spawn non replicated projectile, no need for server side rewind
			{
				pSpawnedActor = GetWorld()->SpawnActor<AProjectile>(m_pServerSideRewindProjectileClass, socketTransform.GetLocation(), rotationFromSocketToTarget, spawnParameters);
				pSpawnedActor->SetServerSideRewind(false);
			}
		}
		else //we are on the client, so we use server side rewind
		{
			if (pInstigatorPawn->IsLocallyControlled()) // spawned on client, locally controlled, so we spawn a non-replicated projectile and we will use server side rewind
			{
				pSpawnedActor = GetWorld()->SpawnActor<AProjectile>(m_pServerSideRewindProjectileClass, socketTransform.GetLocation(), rotationFromSocketToTarget, spawnParameters);
				pSpawnedActor->SetServerSideRewind(true);
				pSpawnedActor->SetSpawnLocation(socketTransform.GetLocation());
				pSpawnedActor->SetInitialVelocity(pSpawnedActor->GetProjectileMovementComponent()->InitialSpeed * pSpawnedActor->GetActorForwardVector());
				pSpawnedActor->SetDamage(m_Damage);
			}
			else // spawned on client, not locally controlled, so we spawn a non-replicated projectile and we will not use server side rewind
			{
				pSpawnedActor = GetWorld()->SpawnActor<AProjectile>(m_pServerSideRewindProjectileClass, socketTransform.GetLocation(), rotationFromSocketToTarget, spawnParameters);
				pSpawnedActor->SetServerSideRewind(false);
				pSpawnedActor->SetSpawnLocation(socketTransform.GetLocation());
				pSpawnedActor->SetInitialVelocity(pSpawnedActor->GetProjectileMovementComponent()->InitialSpeed * pSpawnedActor->GetActorForwardVector());
			}
		}
	}
	else if (pInstigatorPawn->HasAuthority())
	{
		pSpawnedActor = GetWorld()->SpawnActor<AProjectile>(m_pProjectileClass, socketTransform.GetLocation(), rotationFromSocketToTarget, spawnParameters);
		pSpawnedActor->SetServerSideRewind(false);
		pSpawnedActor->SetDamage(m_Damage);
	}
	
}
