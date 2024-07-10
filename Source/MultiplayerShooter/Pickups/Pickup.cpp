// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
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

	m_pPickupFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Pickup FX"));
	m_pPickupFX->SetupAttachment(RootComponent);
}	

void APickup::BeginPlay()
{
	Super::BeginPlay();

	//This is because if the player is already overlapping with the pickup when it spawns, then the overlap doesn't have enough time to be bound and the pickup just gets destroyed.
	if (HasAuthority())
		GetWorldTimerManager().SetTimer(m_DelayOverlapBindingHandle, this, &APickup::DelayOverlapBinding, 0.25f, false);
}

void APickup::Destroyed()
{
	Super::Destroyed();

	if (m_pPickupSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pPickupSound, GetActorLocation());

	if(m_pPickupFXSystem)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), m_pPickupFXSystem, GetActorLocation(), GetActorRotation());

	
}

void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	
}

void APickup::DelayOverlapBinding()
{
	m_pSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnSphereOverlap);
}

