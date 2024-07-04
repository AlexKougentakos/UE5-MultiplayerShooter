#include "ProjectileRocket.h"

#include "Kismet/GameplayStatics.h"

AProjectileRocket::AProjectileRocket()
{
	m_pRocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	m_pRocketMesh->SetupAttachment(RootComponent);
	m_pRocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//This is set in the ProjectileWeapon.cpp when firing the projectile
	const APawn* pFiringPawn = GetInstigator();
	checkf(pFiringPawn, TEXT("No instigator found for projectile"));

	AController* pFiringPawnController = pFiringPawn->GetController();
	checkf(pFiringPawnController, TEXT("No controller found for instigator"));

	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		m_Damage
		,m_Damage * 0.33f
		,GetActorLocation(),
		200.0f,
		500.0f,
		1.0f,
		UDamageType::StaticClass(),
		TArray<AActor*>(),
		this,
		pFiringPawnController);
	
	//Perform at the end since it will destroy the projectile
	Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
}
