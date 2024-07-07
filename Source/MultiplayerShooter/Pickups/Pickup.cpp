// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	m_pSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	m_pSphereComponent->SetupAttachment(RootComponent);
	m_pSphereComponent->SetSphereRadius(150.f);
	m_pSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	m_pSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	m_pSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	m_pPickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	m_pPickupMesh->SetupAttachment(m_pSphereComponent);
	m_pPickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_pPickupMesh->SetRenderCustomDepth(true);
	m_pPickupMesh->SetCustomDepthStencilValue(250);
}	

void APickup::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
		m_pSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnSphereOverlap);
}

void APickup::Destroyed()
{
	Super::Destroyed();

	//checkf(m_pPickupSound, TEXT("Pickup sound is nullptr"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pPickupSound, GetActorLocation());
}

void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	
}

