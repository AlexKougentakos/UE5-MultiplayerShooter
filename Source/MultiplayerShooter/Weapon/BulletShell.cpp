#include "BulletShell.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABulletShell::ABulletShell()
{
	PrimaryActorTick.bCanEverTick = false;

	m_pBulletShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletShellMesh"));
	SetRootComponent(m_pBulletShellMesh);

	//Ignore the camera so it doesn't zoom in when it collides with the bullet shell
	m_pBulletShellMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	//Enable physics & gravity
	m_pBulletShellMesh->SetSimulatePhysics(true);
	m_pBulletShellMesh->SetEnableGravity(true);
	m_pBulletShellMesh->SetNotifyRigidBodyCollision(true);

	//Set the min & max time alive
	m_MinTimeAlive = m_TimeAliveRange.Min;
	m_MaxTimeAlive = m_TimeAliveRange.Max;
}

void ABulletShell::BeginPlay()
{
	Super::BeginPlay();
	
	m_pBulletShellMesh->OnComponentHit.AddDynamic(this, &ABulletShell::OnHit);
	m_pBulletShellMesh->AddImpulse(GetActorForwardVector() * m_ShellEjectionForce);

	SetLifeSpan(FMath::FRandRange(m_MinTimeAlive, m_MaxTimeAlive));
}

void ABulletShell::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	checkf(m_pShellDropSound, TEXT("Shell drop sound is nullptr"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pShellDropSound, GetActorLocation());

	// Deactivate further sounds when a bullet is hit by another one
	m_pBulletShellMesh->SetNotifyRigidBodyCollision(false);
}
