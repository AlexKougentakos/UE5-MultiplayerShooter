// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "BulletShell.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"
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

	m_CurrentAmmo = m_MaxAmmo;
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
	DOREPLIFETIME(AWeapon, m_CurrentAmmo);
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
		m_pWeaponMesh->SetSimulatePhysics(false);
		m_pWeaponMesh->SetEnableGravity(false);
		m_pWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
			m_pAreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		m_pWeaponMesh->SetSimulatePhysics(true);
		m_pWeaponMesh->SetEnableGravity(true);
		m_pWeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EWeaponState::EWS_MAX:
		break;
	}

}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	//this will also be called when the owner becomes null so we need to handle that
	if (!Owner)
	{
		m_pWeaponHolder = nullptr;
		m_pWeaponHolderController = nullptr;
	}
	else UpdateHudAmmo();	
}

void AWeapon::OnRep_Ammo()
{
	UpdateHudAmmo();
}

void AWeapon::SpendAmmoRound()
{
	m_CurrentAmmo = FMath::Clamp(m_CurrentAmmo - 1, 0, m_MaxAmmo);
	
	UpdateHudAmmo();
}

void AWeapon::UpdateHudAmmo()
{
	m_pWeaponHolder = m_pWeaponHolder ? m_pWeaponHolder : Cast<ABlasterCharacter>(GetOwner());
	if (m_pWeaponHolder)
	{
		m_pWeaponHolderController = m_pWeaponHolderController ? m_pWeaponHolderController : Cast<ABlasterPlayerController>(m_pWeaponHolder->GetController());
		if (m_pWeaponHolderController)
		{
			m_pWeaponHolderController->SetHudAmmo(m_CurrentAmmo);
		}
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
		m_pWeaponMesh->SetSimulatePhysics(false);
		m_pWeaponMesh->SetEnableGravity(false);
		m_pWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		m_pWeaponMesh->SetSimulatePhysics(true);
		m_pWeaponMesh->SetEnableGravity(true);
		m_pWeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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
	checkf(m_pBulletShellClass, TEXT("Bullet shell class is nullptr"));
	
	m_pWeaponMesh->PlayAnimation(m_pFireAnimation, false);
	
	const auto pAmmoEjectSocket = m_pWeaponMesh->GetSocketByName(FName("AmmoEject"));

	//Various checks for points that should never be null
	checkf(pAmmoEjectSocket, TEXT("MuzzleFlash socket is nullptr"));
	
	const FTransform socketTransform = pAmmoEjectSocket->GetSocketTransform(m_pWeaponMesh);
	
	const auto pBulletShell = GetWorld()->SpawnActor<ABulletShell>(
	m_pBulletShellClass,
	socketTransform.GetLocation(),
	socketTransform.GetRotation().Rotator(),
	FActorSpawnParameters());

	SpendAmmoRound();
}

int AWeapon::Reload(const int availableAmmo)
{
	const int missingAmmo = m_MaxAmmo - m_CurrentAmmo;
	const int ammoToReload = FMath::Min(availableAmmo, missingAmmo);
	m_CurrentAmmo += ammoToReload;

	UpdateHudAmmo();
	return ammoToReload;
}

void AWeapon::Drop()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	const FDetachmentTransformRules detachRules(EDetachmentRule::KeepWorld, true);
	m_pWeaponMesh->DetachFromComponent(detachRules);

	SetOwner(nullptr);
	m_pWeaponHolder = nullptr;
	m_pWeaponHolderController = nullptr; // If you don't do this the casts above will not update and you will keep a ref to the previous controller
}
