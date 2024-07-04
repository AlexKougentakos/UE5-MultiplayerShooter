// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/MultiplayerShooter.h"
#include "MultiplayerShooter/GameModes/BlasterGameMode.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"
#include "MultiplayerShooter/PlayerState/BlasterPlayerState.h"
#include "MultiplayerShooter/Types/TurningInPlace.h"
#include "MultiplayerShooter/Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//Set up the camera
	m_pCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	m_pCameraBoom->SetupAttachment(GetMesh());
	m_pCameraBoom->TargetArmLength = 600.0f;
	m_pCameraBoom->bUsePawnControlRotation = true;

	m_pFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	m_pFollowCamera->SetupAttachment(m_pCameraBoom, USpringArmComponent::SocketName);
	m_pFollowCamera->bUsePawnControlRotation = false;

	//Set up the overhead widget
	m_pOverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	m_pOverheadWidget->SetupAttachment(RootComponent);

	//Set up components
	m_pCombat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	m_pCombat->SetIsReplicated(true);
	
	m_pDissolveTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline"));
	
	//Set up the collisions
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	
	//Set up variables that need to be initialized
	m_TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	m_CurrentHealth = m_MaxHealth;
	m_IsAlive = true;
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	m_pPlayerController = Cast<ABlasterPlayerController>(GetController());

	
	if (m_pPlayerController)
	{
		m_pPlayerController->ShowAmmo(false);
		m_pPlayerController->SetHudHealth(m_CurrentHealth, m_MaxHealth);
	}

	if (HasAuthority())
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
}

void ABlasterCharacter::PollInitialize(float deltaTime)
{
	if (m_PollInitializedComplete) return;

	if (!m_pBlasterPlayerState)
	{
		m_pBlasterPlayerState = Cast<ABlasterPlayerState>(GetPlayerState());
		if (m_pBlasterPlayerState)
		{
			m_pBlasterPlayerState->AddToSore(0);
			m_pBlasterPlayerState->AddToDeaths(0);
		}
		return;
	}
	
	//Keep this at the bottom
	m_ElapsedPollingTime += deltaTime;
	if (m_ElapsedPollingTime > m_MaxPollingTime)
	{
		UE_LOG(LogTemp, Error, TEXT("Polling initialization took too long"));
		GEngine->AddOnScreenDebugMessage(1947, 5.f, FColor::Red, TEXT("Polling initialization took too long"));
		m_PollInitializedComplete = true;
	}
	m_PollInitializedComplete = true;
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PollInitialize(DeltaTime);
	
	if (GetLocalRole() > ROLE_SimulatedProxy && IsLocallyControlled())
	{
		CalculateAimOffset(DeltaTime);
	}
	// The reason for this is that OnRep_ReplicatedMovement only gets called when there's a change in the movement of the character
	// So if the character is standing still, the replicated movement won't get called and the proxies won't update their rotation
	// This is a relatively cheap call
	else
	{
		m_TimeSinceLastMovementReplication += DeltaTime;
		if (m_TimeSinceLastMovementReplication >  0.15f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAimOffsetPitch();
	}
	
	HideCameraWhenPlayerIsClose();
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
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABlasterCharacter::ReloadButtonPressed);

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
	DOREPLIFETIME(ABlasterCharacter, m_CurrentHealth);
	DOREPLIFETIME(ABlasterCharacter, m_DisabledGameplay);
}

void ABlasterCharacter::Jump()
{
	if (bIsCrouched) UnCrouch();
	
	else Super::Jump();
}

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();

	// We do this in the destroyed function since it gets replicated to all clients
	if (m_pEliminationBotEffectComponent) m_pEliminationBotEffectComponent->DestroyComponent();
}

void ABlasterCharacter::Eliminated()
{
	checkf(m_pCombat, TEXT("Combat component is nullptr"));

	if (m_pCombat->HasWeapon())
		m_pCombat->m_pEquippedWeapon->Drop();
	
	MulticastEliminated();
	GetWorldTimerManager().SetTimer(m_EliminationTimer, this, &ABlasterCharacter::EliminationTimerFinished, m_RespawnTimer);
}

void ABlasterCharacter::MulticastEliminated_Implementation()
{
	if (m_pPlayerController) m_pPlayerController->ShowAmmo(false);
	
	m_IsAlive = false;
	PlayEliminationMontage();

	// Play dissolve effect
	checkf(m_pDissolveMaterialInstance, TEXT("Dissolve material instance is nullptr"));
	m_pDynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(m_pDissolveMaterialInstance, this);

	GetMesh()->SetMaterial(0, m_pDynamicDissolveMaterialInstance);
	m_pDynamicDissolveMaterialInstance->SetScalarParameterValue("DissolveAmount", 0.55f);
	m_pDynamicDissolveMaterialInstance->SetScalarParameterValue("Glow Intensity", 30.f);

	StartDissolve();

	//Disable Character Movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	if(m_pPlayerController)
		DisableInput(m_pPlayerController);

	//Disable Collisions
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Spawn the elimination bot effect
	checkf(m_pEliminationBotEffect, TEXT("Elimination bot effect is nullptr"));
	const FVector location = GetActorLocation();
	const FVector eliminationBotEffectLocation = FVector{location.X, location.Y, location.Z + 200.f};
	m_pEliminationBotEffectComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_pEliminationBotEffect, eliminationBotEffectLocation, GetActorRotation());

	checkf(m_pEliminationSound, TEXT("Elimination sound is nullptr"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pEliminationSound, GetActorLocation());
}

void ABlasterCharacter::EliminationTimerFinished()
{
	GetWorld()->GetAuthGameMode<ABlasterGameMode>()->RequestRespawn(this, m_pPlayerController);
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
	const FName sectionName = isAiming ? "Rifle Ironsights" : "Rifle Hip";
	pAnimInstance->Montage_JumpToSection(sectionName);
}

void ABlasterCharacter::PlayHitReactMontage() const
{	
	if (!m_pCombat->HasWeapon()) return;
		
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	checkf(pAnimInstance, TEXT("AnimInstance is nullptr"));
	checkf(m_pHitReactMontage, TEXT("Hit React Montage is nullptr"));

	pAnimInstance->Montage_Play(m_pHitReactMontage, 1.f);
	const FName sectionName = "FromFront";
	pAnimInstance->Montage_JumpToSection(sectionName); 
}

void ABlasterCharacter::PlayEliminationMontage() const
{	
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	checkf(pAnimInstance, TEXT("AnimInstance is nullptr"));
	checkf(m_pEliminationMontage, TEXT("Hit React Montage is nullptr"));

	pAnimInstance->Montage_Play(m_pEliminationMontage, 1.f);
}

void ABlasterCharacter::PlayRifleReloadMontage() const
{
	checkf(m_pCombat, TEXT("Combat component is nullptr"));

	if (!m_pCombat->HasWeapon()) return;

	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	
	checkf (pAnimInstance, TEXT("AnimInstance is nullptr"));
	checkf(m_pReloadMontage, TEXT("Fire Weapon Montage is nullptr"));

	// Play the montage and then decide which section to play depending on the weapon
	pAnimInstance->Montage_Play(m_pReloadMontage, 1.f);
	FName sectionName{};
	switch(m_pCombat->m_pEquippedWeapon->GetWeaponType())
	{
	case EWeaponType::EWT_Rifle:
		sectionName = "Rifle";
		break;
	case EWeaponType::EWT_Pistol:
		break;
	case EWeaponType::EWT_Shotgun:
		break;
	case EWeaponType::EWT_MAX:
		break;
	}
	pAnimInstance->Montage_JumpToSection(sectionName);
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	SimulateProxiesTurn();
	m_TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::MulticastHit_Implementation()
{
	PlayHitReactMontage();
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
	if (!m_pCombat || m_DisabledGameplay) return;
	
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

void ABlasterCharacter::CalculateAimOffsetPitch()
{
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

void ABlasterCharacter::CalculateAimOffset(float deltaTime)
{
	if (!m_pCombat->HasWeapon()) return;
	
	const FVector velocity = GetVelocity();
	const FVector lateralVelocity = FVector{velocity.X, velocity.Y, 0.f};
	const float speed = lateralVelocity.Size();
	const bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (speed < 0.1f && !bIsInAir) // STATE: Standing still & not jumping
	{
		m_RotateRootBone = true;
		const FRotator aimRotation = FRotator{0.f, GetBaseAimRotation().Yaw, 0.f};
		const FRotator deltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(aimRotation, m_LastFrameRotation);
		m_AimOffsetYaw = deltaAimRotation.Yaw;
		if (m_TurningInPlace == ETurningInPlace::ETIP_NotTurning)
			m_InterpolatedAimOffsetYaw = m_AimOffsetYaw;
		bUseControllerRotationYaw = true;
		TurnInPlace(deltaTime);
	}
	else if(speed > 0.1f || bIsInAir)//STATE: Moving or jumping
	{
		m_RotateRootBone = false;
		m_LastFrameRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		m_AimOffsetYaw = 0.f;
		bUseControllerRotationYaw = true;
		m_TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAimOffsetPitch();
}

void ABlasterCharacter::SimulateProxiesTurn()
{
	checkf(m_pCombat, TEXT("Combat component is nullptr"));
	if (!m_pCombat->HasWeapon()) return;

	m_RotateRootBone = false;
	
	const FVector velocity = GetVelocity();
	const FVector lateralVelocity = FVector{velocity.X, velocity.Y, 0.f};
	const float speed = lateralVelocity.Size();

	if (speed > 0.f || GetCharacterMovement()->IsFalling())
	{
		m_TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	m_ProxyRotationLastFrame = m_CurrentProxyRotation;
	m_CurrentProxyRotation = GetActorRotation();
	const float proxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(m_CurrentProxyRotation, m_ProxyRotationLastFrame).Yaw;
	
	if (FMath::Abs(proxyYaw) > m_TurnThreshold)
	{
		if (proxyYaw > m_TurnThreshold)
			m_TurningInPlace = ETurningInPlace::ETIP_Right;
		else if (proxyYaw < -m_TurnThreshold)
			m_TurningInPlace = ETurningInPlace::ETIP_Left;
		else
			m_TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		
		return;	
	}
	
	m_TurningInPlace = ETurningInPlace::ETIP_NotTurning;
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
	if (m_DisabledGameplay) return;
	checkf(m_pCombat, TEXT("Combat component is nullptr"));
	
	m_pCombat->FireButtonPressed(true);
}

void ABlasterCharacter::FireButtonReleased()
{
	if (m_DisabledGameplay) return;
	checkf(m_pCombat, TEXT("Combat component is nullptr"));
	
	m_pCombat->FireButtonPressed(false);
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if (m_DisabledGameplay) return;
	checkf(m_pCombat, TEXT("Combat component is nullptr"));
	m_pCombat->Reload();
}

void ABlasterCharacter::ReceiveDamage(AActor* damagedActor, float damage, const UDamageType* damageType,
                                      AController* instigatedBy, AActor* damageCauser)
{
	m_CurrentHealth = FMath::Clamp(m_CurrentHealth - damage, 0.f, m_MaxHealth);
	m_pPlayerController = m_pPlayerController ? m_pPlayerController : Cast<ABlasterPlayerController>(GetController());
	if (m_pPlayerController)
		m_pPlayerController->SetHudHealth(m_CurrentHealth, m_MaxHealth);
	
	PlayHitReactMontage();

	const auto pBlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();

	if (m_CurrentHealth <= 0.f && pBlasterGameMode)
	{
		m_pPlayerController = m_pPlayerController ? m_pPlayerController : Cast<ABlasterPlayerController>(GetController());
		
		checkf(m_pPlayerController, TEXT("Player controller is nullptr"));
		checkf(instigatedBy, TEXT("InstigatedBy is nullptr"));
		
		pBlasterGameMode->PlayerEliminated(this, m_pPlayerController, Cast<ABlasterPlayerController>(instigatedBy));
	}
}

void ABlasterCharacter::HideCameraWhenPlayerIsClose()
{
	if (!IsLocallyControlled()) return;

	if (FVector::Dist(m_pFollowCamera->GetComponentLocation(), GetActorLocation()) < m_PlayerHideDistance)
	{
		GetMesh()->SetVisibility(false);
		if (m_pCombat->HasWeapon())
		{
			m_pCombat->m_pEquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (m_pCombat->HasWeapon())
		{
			m_pCombat->m_pEquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::OnRep_Health()
{
	PlayHitReactMontage();
	m_pPlayerController = m_pPlayerController ? m_pPlayerController : Cast<ABlasterPlayerController>(GetController());
	if (m_pPlayerController)
		m_pPlayerController->SetHudHealth(m_CurrentHealth, m_MaxHealth);
}

void ABlasterCharacter::UpdateDissolveMaterial(const float dissolveValue)
{
	checkf(m_pDynamicDissolveMaterialInstance, TEXT("Dynamic dissolve material instance is nullptr"));
	m_pDynamicDissolveMaterialInstance->SetScalarParameterValue("DissolveAmount", dissolveValue);
}

void ABlasterCharacter::StartDissolve()
{
	m_DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);
	checkf(m_pDissolveCurve, TEXT("Dissolve curve is nullptr"));

	m_pDissolveTimeLine->AddInterpFloat(m_pDissolveCurve, m_DissolveTrack);
	m_pDissolveTimeLine->Play();
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

ECombatState ABlasterCharacter::GetCombatState() const
{
	checkf(m_pCombat, TEXT("Combat component is nullptr"));
	return m_pCombat->m_CombatState;
}

