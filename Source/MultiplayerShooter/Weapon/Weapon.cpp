// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	m_pWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	m_pWeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(m_pWeaponMesh);

	m_pWeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	m_pWeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	m_pWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	m_pAreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	m_pAreaSphere->SetupAttachment(RootComponent);

	// Ignore all collision for clients and we will enable it for the server
	m_pAreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore); 
	m_pAreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	m_pPickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pick Up Widget"));
	m_pPickUpWidget->SetupAttachment(RootComponent);	
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		// Activate sphere in the server
		m_pAreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		m_pAreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		m_pAreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		m_pAreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereOverlapEnd);
	}

	if (m_pPickUpWidget)
	{
		m_pPickUpWidget->SetVisibility(false);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, m_WeaponState);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* const pCharacter = Cast<ABlasterCharacter>(OtherActor);
	
	if (pCharacter)
	{
		pCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* const pCharacter = Cast<ABlasterCharacter>(OtherActor);
	
	if (pCharacter)
	{
		pCharacter->SetOverlappingWeapon(nullptr);
	}	
}

void AWeapon::OnRep_WeaponState() const
{
	switch (m_WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_MAX:
		break;
	}
}

void AWeapon::SetWeaponState(const EWeaponState state)
{
	//We change the state here to call the OnRep_WeaponState function
	m_WeaponState = state;

	switch (m_WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		m_pAreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_MAX:
		break;
	}

}

void AWeapon::ShowPickupWidget(bool show) const
{
	if (!m_pPickUpWidget) return;

	m_pPickUpWidget->SetVisibility(show);
}

void AWeapon::Fire(const FVector& hitTarget)
{
	checkf(m_pFireAnimation, TEXT("Fire animation is nullptr"));

	m_pWeaponMesh->PlayAnimation(m_pFireAnimation, false);
}
