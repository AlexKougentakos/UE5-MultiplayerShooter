#include "Shotgun.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
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

void AShotgun::Fire(const FVector& hitTarget)
{
	AWeapon::Fire(hitTarget);
	
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
		const FVector end = GetVectorWithSpread(start, hitTarget);

		// Assign a hit values to each character that you hit (can be multiple due to the spread)
		// and apply damage to them ONCE at the end
		TMap<ABlasterCharacter*, int> hitMap{};
		for (int i = 0; i < m_NumberOfPellets; ++i)
		{
			FHitResult hitResult{};
			WeaponTraceHit(start, end, hitResult);

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
		for (const auto& pair : hitMap)
		{
			if (HasAuthority() && pOwnerController)
				UGameplayStatics::ApplyDamage(pair.Key, m_Damage * pair.Value, pOwnerController, this, UDamageType::StaticClass());
		}
	}
	
}

