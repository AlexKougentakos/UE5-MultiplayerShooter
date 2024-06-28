// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
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
	
	m_TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

}


void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateAimOffset(DeltaTime);
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed,this, &ABlasterCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);

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
	
	DOREPLIFETIME_CONDITION(ABlasterCharacter, m_pOverlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::Jump()
{
	if (bIsCrouched) UnCrouch();
	
	else Super::Jump();
}

void ABlasterCharacter::PlayFireMontage(const bool isAiming) const
{
	checkf(m_pCombat, TEXT("Combat component is nullptr"));

	if (!m_pCombat->HasWeapon()) return;

	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	
	checkf (pAnimInstance, TEXT("AnimInstance is nullptr"));
	checkf(m_pFireWeaponMontage, TEXT("Fire Weapon Montage is nullptr"));

	// Play the montage and then decide which section to play depending on if we are using iron sights or hip fire
	pAnimInstance->Montage_Play(m_pFireWeaponMontage, 1.f);
	const FName sectionName = isAiming ? "RifleAim" : "RifleHip";
	pAnimInstance->Montage_JumpToSection(sectionName);
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

void ABlasterCharacter::CalculateAimOffset(float deltaTime)
{
	if (!m_pCombat->HasWeapon()) return;
	
	const FVector velocity = GetVelocity();
	const FVector lateralVelocity = FVector{velocity.X, velocity.Y, 0.f};
	const float speed = lateralVelocity.Size();
	const bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (speed < 0.1f && !bIsInAir) // STATE: Standing still & not jumping
	{
		const FRotator aimRotation = FRotator{0.f, GetBaseAimRotation().Yaw, 0.f};
		const FRotator deltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(aimRotation, m_LastFrameRotation);
		m_AimOffsetYaw = deltaAimRotation.Yaw;
		if (m_TurningInPlace == ETurningInPlace::ETIP_NotTurning)
			m_InterpolatedAimOffsetYaw = m_AimOffsetYaw;
		bUseControllerRotationYaw = true;
		TurnInPlace(deltaTime);
	}
	else //STATE: Moving or jumping
	{
		m_LastFrameRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		m_AimOffsetYaw = 0.f;
		bUseControllerRotationYaw = true;
		m_TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	// Now we set the pitch regardless of if the player is moving/jumping or not
	m_AimOffsetPitch = GetBaseAimRotation().Pitch;

	// UE5 packages the pitch to send more efficiently over the network when replicating but it becomes a unsigned int
	// so our negative values need to be adjusted. This is not needed when locally controlled because we are not sending
	// data over the network
	if (m_AimOffsetPitch > 90.f && !IsLocallyControlled())
	{
		// map [270, 360) to [-90, 0)
		const FVector2d inRange = FVector2d{270.f, 360.f};
		const FVector2d outRange = FVector2d{-90.f, 0.f};
		m_AimOffsetPitch = FMath::GetMappedRangeValueClamped(inRange, outRange, m_AimOffsetPitch);
	}
}

void ABlasterCharacter::TurnInPlace(float deltaTime)
{
	if (m_AimOffsetYaw > m_StandingPlayerRotationAngle)
	{
		m_TurningInPlace = ETurningInPlace::ETIP_Right;
		UE_LOG(LogTemp, Warning, TEXT("Turning Right"));
	}
	else if (m_AimOffsetYaw < -m_StandingPlayerRotationAngle)
		m_TurningInPlace = ETurningInPlace::ETIP_Left;
	
	if (m_TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		m_InterpolatedAimOffsetYaw = FMath::FInterpTo(m_InterpolatedAimOffsetYaw, 0.f, deltaTime, m_RotationSpeed);
		m_AimOffsetYaw = m_InterpolatedAimOffsetYaw;
		if (FMath::Abs(m_AimOffsetYaw) < 10.f)
		{
			m_TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			m_LastFrameRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ABlasterCharacter::FireButtonPressed()
{
	checkf(m_pCombat, TEXT("Combat component is nullptr"));
	
	m_pCombat->FireButtonPressed(true);
}

void ABlasterCharacter::FireButtonReleased()
{
	checkf(m_pCombat, TEXT("Combat component is nullptr"));
	
	m_pCombat->FireButtonPressed(false);
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

AWeapon* ABlasterCharacter::GetEquippedWeapon() const
{
	return m_pCombat->m_pEquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	checkf(m_pCombat, TEXT("Combat component is nullptr"));
	return m_pCombat->m_HitTarget;
}

