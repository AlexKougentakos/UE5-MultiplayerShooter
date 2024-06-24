#include "Projectile.h"

#include "Components/BoxComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	m_pCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(m_pCollisionBox);

	//Set up collisions
	m_pCollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	m_pCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	m_pCollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	m_pCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

