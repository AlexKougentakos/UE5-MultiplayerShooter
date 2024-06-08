// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MultiplayerShooter/Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	m_pCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	m_pCameraBoom->SetupAttachment(GetMesh());
	m_pCameraBoom->TargetArmLength = 600.0f;
	m_pCameraBoom->bUsePawnControlRotation = true;

	m_pFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	m_pFollowCamera->SetupAttachment(m_pCameraBoom, USpringArmComponent::SocketName);
	m_pFollowCamera->bUsePawnControlRotation = false;

	m_pOverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	m_pOverheadWidget->SetupAttachment(RootComponent);

	m_pCombat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	m_pCombat->SetIsReplicated(true);
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed,this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	m_pCombat->m_pCharacter = this;
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, m_pOverlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::MoveForward(const float value)
{
	if (!Controller || value == 0.0f) return;

	const FRotator yawRotation{0, Controller->GetControlRotation().Yaw, 0};
	const FVector direction = FRotationMatrix{yawRotation}.GetUnitAxis(EAxis::X);
	AddMovementInput(direction, value);
}

void ABlasterCharacter::MoveRight(const float value)
{
	if (!Controller || value == 0.0f) return;

	const FRotator yawRotation{0, Controller->GetControlRotation().Yaw, 0};
	const FVector direction = FRotationMatrix{yawRotation}.GetUnitAxis(EAxis::Y);
	AddMovementInput(direction, value);
}

void ABlasterCharacter::Turn(const float value)
{
	AddControllerYawInput(value);
}

void ABlasterCharacter::LookUp(const float value)
{
	AddControllerPitchInput(value);
}

void ABlasterCharacter::EquipButtonPressed()
{	
	if (!m_pCombat) return;
	
	if (HasAuthority()) //Server 
		m_pCombat->EquipWeapon(m_pOverlappingWeapon);
	else //Client
		ServerEquipButtonPressed();
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bIsCrouched)
		UnCrouch();
	else
		Crouch();
}

void ABlasterCharacter::AimButtonPressed()
{
	if (!m_pCombat) return;
	
	m_pCombat->SetAiming(true);
}

void ABlasterCharacter::AimButtonReleased()
{
	if (!m_pCombat) return;

	m_pCombat->SetAiming(false);
}

void ABlasterCharacter::OnRep_OverlappingWeapon(const AWeapon* const pOldWeapon) const
{
	if (m_pOverlappingWeapon)
		m_pOverlappingWeapon->ShowPickupWidget(true);

	if (pOldWeapon)
		pOldWeapon->ShowPickupWidget(false);
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (!m_pCombat) return;

	m_pCombat->EquipWeapon(m_pOverlappingWeapon);
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* const pWeapon)
{
	if (IsLocallyControlled() && m_pOverlappingWeapon)
	{
		m_pOverlappingWeapon->ShowPickupWidget(false);
	}
	
	m_pOverlappingWeapon = pWeapon;

	//This function only gets called in the server, so if it is locally controlled we know that we are on the server
	if (IsLocallyControlled() && m_pOverlappingWeapon)
	{		
		m_pOverlappingWeapon->ShowPickupWidget(true);
	}
}

bool ABlasterCharacter::IsWeaponEquipped() const
{
	return (m_pCombat && m_pCombat->m_pEquippedWeapon);
}

bool ABlasterCharacter::IsAiming() const
{
	return (m_pCombat && m_pCombat->m_IsAiming);
}

