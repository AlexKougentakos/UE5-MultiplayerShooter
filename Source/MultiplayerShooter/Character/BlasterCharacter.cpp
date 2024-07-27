#include "BlasterCharacter.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/LagCompensationComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/MultiplayerShooter.h"
#include "MultiplayerShooter/GameModes/BlasterGameMode.h"
#include "MultiplayerShooter/GameModes/LobbyGameMode.h"
#include "MultiplayerShooter/GameState/BlasterGameState.h"
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

	m_pBuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	m_pBuffComponent->SetIsReplicated(true);

	m_pLagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensationComponent"));
	
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

	//Set up collision boxes
	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), "head");
	head->SetCollisionResponseToAllChannels(ECR_Ignore);
	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), "pelvis");
	pelvis->SetCollisionResponseToAllChannels(ECR_Ignore);
	spine_01 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_01"));
	spine_01->SetupAttachment(GetMesh(), "spine_01");
	spine_01->SetCollisionResponseToAllChannels(ECR_Ignore);
	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), "spine_02");
	spine_02->SetCollisionResponseToAllChannels(ECR_Ignore);
	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), "upperarm_l");
	upperarm_l->SetCollisionResponseToAllChannels(ECR_Ignore);
	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), "upperarm_r");
	upperarm_r->SetCollisionResponseToAllChannels(ECR_Ignore);
	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), "lowerarm_l");
	lowerarm_l->SetCollisionResponseToAllChannels(ECR_Ignore);
	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), "lowerarm_r");
	lowerarm_r->SetCollisionResponseToAllChannels(ECR_Ignore);
	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), "hand_l");
	hand_l->SetCollisionResponseToAllChannels(ECR_Ignore);
	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), "hand_r");
	hand_r->SetCollisionResponseToAllChannels(ECR_Ignore);
	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), "thigh_l");
	thigh_l->SetCollisionResponseToAllChannels(ECR_Ignore);
	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), "thigh_r");
	thigh_r->SetCollisionResponseToAllChannels(ECR_Ignore);
	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), "calf_l");
	calf_l->SetCollisionResponseToAllChannels(ECR_Ignore);
	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), "calf_r");
	calf_r->SetCollisionResponseToAllChannels(ECR_Ignore);
	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), "foot_l");
	foot_l->SetCollisionResponseToAllChannels(ECR_Ignore);
	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), "foot_r");
	foot_r->SetCollisionResponseToAllChannels(ECR_Ignore);
	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), "backpack");
	backpack->SetCollisionResponseToAllChannels(ECR_Ignore);

	HitBoxes.Add("foot_l", foot_l);
	HitBoxes.Add("foot_r", foot_r);
	HitBoxes.Add("backpack", backpack);
	HitBoxes.Add("calf_r", calf_r);
	HitBoxes.Add("calf_l", calf_l);
	HitBoxes.Add("thigh_r", thigh_r);
	HitBoxes.Add("thigh_l", thigh_l);
	HitBoxes.Add("hand_r", hand_r);
	HitBoxes.Add("hand_l", hand_l);
	HitBoxes.Add("lowerarm_r", lowerarm_r);
	HitBoxes.Add("lowerarm_l", lowerarm_l);
	HitBoxes.Add("upperarm_r", upperarm_r);
	HitBoxes.Add("head", head);
	HitBoxes.Add("pelvis", pelvis);
	HitBoxes.Add("spine_01", spine_01);
	HitBoxes.Add("spine_02", spine_02);
	HitBoxes.Add("upperarm_l", upperarm_l);
	
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	m_pPlayerController = Cast<ABlasterPlayerController>(GetController());
	
	if (m_pPlayerController)
	{
		UpdateHudHealth();
		UpdateHudShield();
	}

	if (HasAuthority())
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);

	
	SpawnDefaultWeapon();
	UpdateHudAmmo();
	m_pCombat->UpdateWeaponHUD();
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
			
			ABlasterGameState* pGameState = GetWorld()->GetGameState<ABlasterGameState>();
			if (pGameState && pGameState->GetTopScoringPlayers().Contains(m_pBlasterPlayerState))
			{
				MulticastGainedTheLead();
			}
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
	TraceForWeaponPickup();
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
	PlayerInputComponent->BindAction("OpenChat", IE_Pressed, this, &ABlasterCharacter::EnterChatTyping);
	//PlayerInputComponent->BindAction("GrenadeThrow", IE_Pressed, this, &ABlasterCharacter::GrenadeThrowButtonPressed);
	//The animation replication was very bugged and I couldn't get it fixed. I am leaving the logic in the code in case
	//I can fix it later

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	m_pCombat->m_pCharacter = this;
	m_pBuffComponent->m_pCharacter = this;
	m_pLagCompensation->m_pCharacter = this;

	if (Controller) m_pLagCompensation->m_pBlasterPlayerController = Cast<ABlasterPlayerController>(Controller);
	
	m_pBuffComponent->m_InitialBaseSpeed = GetCharacterMovement()->MaxWalkSpeed;
	m_pBuffComponent->m_InitialCrouchedSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;
	m_pBuffComponent->m_InitialJumpVelocity = GetCharacterMovement()->JumpZVelocity;
	
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ABlasterCharacter, m_pOverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, m_CurrentHealth);
	DOREPLIFETIME(ABlasterCharacter, m_CurrentShield);
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

void ABlasterCharacter::Eliminated(const bool playerLeftGame)
{
	
	checkf(m_pCombat, TEXT("Combat component is nullptr"));
	if (m_pCombat->HasWeapon())
	{
		if (m_pCombat->m_pEquippedWeapon->ShouldDestroyWeapon())
			m_pCombat->m_pEquippedWeapon->Destroy();
		else
			m_pCombat->m_pEquippedWeapon->Drop();
	}
	if (m_pCombat->HasSecondaryWeapon()) m_pCombat->m_pSecondaryWeapon->Drop();
	m_pCombat->UpdateWeaponHUD();
	
	MulticastEliminated(playerLeftGame);
}

void ABlasterCharacter::MulticastEliminated_Implementation(const bool playerLeftGame)
{
	m_PlayerLeftGame = playerLeftGame;
	
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

	m_pCombat->FireButtonPressed(false);

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

	//Hide the crown if we have it
	if (m_pTopKillsCrownEffectComponent) m_pTopKillsCrownEffectComponent->Deactivate();

	// Hide the sniper scope widget
	if (IsLocallyControlled() &&
		m_pCombat->HasWeapon() &&
		m_pCombat->m_pEquippedWeapon->GetWeaponType() == EWeaponType::EWT_Sniper
		&& m_pCombat->m_IsAiming) ShowSniperScopeWidget(false);
	
	GetWorldTimerManager().SetTimer(m_EliminationTimer, this, &ABlasterCharacter::EliminationTimerFinished, m_RespawnTimer);
}

void ABlasterCharacter::EliminationTimerFinished()
{
	if (m_PlayerLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
		return;
	}

	const auto pGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (!m_PlayerLeftGame && pGameMode)
		pGameMode->RequestRespawn(this, m_pPlayerController);
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
	checkf(m_pEliminationMontage, TEXT("Elimination Montage is nullptr"));

	pAnimInstance->Montage_Play(m_pEliminationMontage, 1.f);
}

void ABlasterCharacter::PlayThrowGrenadeMontage() const
{
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	checkf(pAnimInstance, TEXT("AnimInstance is nullptr"));
	checkf(m_pGrenadeThrowMontage, TEXT("Grenade Throw Montage is nullptr"));

	pAnimInstance->Montage_Play(m_pGrenadeThrowMontage, 1.f);
}

void ABlasterCharacter::PlayWeaponSwapMontage() const
{
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	checkf(pAnimInstance, TEXT("AnimInstance is nullptr"));
	checkf(m_pWeaponSwapMontage, TEXT("Weapon Swap Montage is nullptr"));

	pAnimInstance->Montage_Play(m_pWeaponSwapMontage, 1.f);
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
	case EWeaponType::EWT_RocketLauncher:
		sectionName = "Rocket";
		break;
	case EWeaponType::EWT_Pistol:
		sectionName = "Pistol";
		break;
	case EWeaponType::EWT_SubmachineGun:
		sectionName = "Pistol";
		break;
	case EWeaponType::EWT_Sniper:
		sectionName = "Sniper";
		break;
	case EWeaponType::EWT_Shotgun:
		sectionName = "Shotgun";
		break;
	case EWeaponType::EWT_GrenadeLauncher:
		sectionName = "Grenade";
		break;
	default: ;
	}
	pAnimInstance->Montage_JumpToSection(sectionName);
}


void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	SimulateProxiesTurn();
	m_TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::UpdateHudHealth()
{
	m_pPlayerController = m_pPlayerController ? m_pPlayerController : Cast<ABlasterPlayerController>(GetController());
	if (m_pPlayerController)
		m_pPlayerController->SetHudHealth(m_CurrentHealth, m_MaxHealth);
}

void ABlasterCharacter::UpdateHudShield()
{
	m_pPlayerController = m_pPlayerController ? m_pPlayerController : Cast<ABlasterPlayerController>(GetController());
	if (m_pPlayerController)
		m_pPlayerController->SetHudShield(m_CurrentShield, m_MaxShield);
}

void ABlasterCharacter::UpdateHudAmmo()
{
	m_pPlayerController = m_pPlayerController ? m_pPlayerController : Cast<ABlasterPlayerController>(GetController());
	if (m_pPlayerController && m_pCombat->HasWeapon())
	{
		m_pPlayerController->SetHudAmmo(m_pCombat->m_pEquippedWeapon->GetCurrentAmmo());
		m_pPlayerController->SetHudCarriedAmmo(m_pCombat->m_CarriedAmmo);
	}
	
}

void ABlasterCharacter::UpdateHudWeapons()
{
	if (!m_pCombat) return;
	
	m_pCombat->UpdateWeaponHUD();
}

void ABlasterCharacter::SpawnDefaultWeapon()
{
	const ABlasterGameMode* pGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (pGameMode)
	{
		checkf(m_DefaultWeaponClass, TEXT("Default weapon is nullptr"));
		AWeapon* pStartingWeapon = GetWorld()->SpawnActor<AWeapon>(m_DefaultWeaponClass);
		pStartingWeapon->SetShouldDestroyWeapon(true);
		m_pCombat->EquipWeapon(pStartingWeapon);
	}
}

void ABlasterCharacter::MulticastGainedTheLead_Implementation()
{
	if (!m_pTopKillsCrownEffect) return;
	if (!m_pTopKillsCrownEffectComponent)
		m_pTopKillsCrownEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			m_pTopKillsCrownEffect,
			GetCapsuleComponent(),
			FName(),
			GetActorLocation() + FVector{0.f, 0.f, 100.f},
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false);

	m_pTopKillsCrownEffectComponent->Activate();
}

void ABlasterCharacter::MulticastLostTheLead_Implementation()
{
	if (m_pTopKillsCrownEffectComponent) m_pTopKillsCrownEffectComponent->Deactivate();
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
	
	if (m_pCombat->m_CombatState == ECombatState::ECS_Unoccupied)
	{
		ServerEquipButtonPressed();
	}
	
	if (m_pCombat->ShouldSwapWeapons() && !HasAuthority() && m_pOverlappingWeapon == nullptr)
	{
		PlayWeaponSwapMontage();
		m_pCombat->m_CombatState = ECombatState::ECS_SwappingWeapons;
		m_FinishedSwappingWeapons = false;
	}
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

void ABlasterCharacter::GrenadeThrowButtonPressed()
{
	if (!m_pCombat || m_DisabledGameplay) return;

	m_pCombat->ThrowGrenade();
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

void ABlasterCharacter::EnterChatTyping()
{
	if (m_pPlayerController)
	{
		m_pPlayerController->ChatOpened();
	}
}

void ABlasterCharacter::ExitChatTyping()
{
	if (m_pPlayerController)
	{
		m_pPlayerController->SetInputMode(FInputModeGameOnly());
		m_pPlayerController->bShowMouseCursor = false;
		
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* damagedActor, float damage, const UDamageType* damageType,
                                      AController* instigatedBy, AActor* damageCauser)
{
	//Snipers can break the shield with a headshot
	const AWeapon* pDamagingWeapon = Cast<AWeapon>(damageCauser);
	if (pDamagingWeapon &&  pDamagingWeapon->GetWeaponType() == EWeaponType::EWT_Sniper && damageType && damageType->IsA<UHeadshotDamageType>())
	{
		const float shieldDamage = FMath::Clamp(m_CurrentShield - damage, 0.f, m_MaxShield);
		const float healthDamage = FMath::Clamp(m_CurrentHealth - (damage - shieldDamage), 0.f, m_MaxHealth);
		m_CurrentShield = shieldDamage;
		m_CurrentHealth = healthDamage;
	}
	else
	{
		if (m_CurrentShield <= 0 ) m_CurrentHealth = FMath::Clamp(m_CurrentHealth - damage, 0.f, m_MaxHealth);
		else m_CurrentShield = FMath::Clamp(m_CurrentShield - damage, 0.f, m_MaxShield);	
	}

	UpdateHudHealth();
	UpdateHudShield();
	UpdateHudAmmo();
	
	if (GetCombatState() == ECombatState::ECS_Reloading)
	{
		m_pCombat->m_CombatState = ECombatState::ECS_Unoccupied;
		m_pCombat->m_IsLocallyReloading = false;
	}

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

void ABlasterCharacter::TraceForWeaponPickup()
{
	if (!IsLocallyControlled()) return;

	FHitResult hitResult{};
	FVector2D viewport{};
	GEngine->GameViewport->GetViewportSize(viewport);

	const FVector2D crosshairLocation = viewport / 2.0f;
	FVector crosshairWorldPosition{};
	FVector crosshairWorldDirection{};

	const bool screenToWorldSucceeded =
	   UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(GetWorld(), 0), crosshairLocation, crosshairWorldPosition, crosshairWorldDirection);

	if (!screenToWorldSucceeded) return;

	FVector start = crosshairWorldPosition;
    
	const float distanceToCharacter = FVector::Dist(start, GetActorLocation());
	start += crosshairWorldDirection * (distanceToCharacter + 50.f);
    
	const FVector end = start + crosshairWorldDirection * m_PickUpDistance;
	GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ETC_PickUp);

	if (hitResult.bBlockingHit)
	{
		AWeapon* pWeapon = Cast<AWeapon>(hitResult.GetActor());
		if (pWeapon)
		{
			ServerSetOverlappingWeapon(pWeapon);
		}
	}
	else if (m_pOverlappingWeapon)
	{
		ServerSetOverlappingWeapon(nullptr);
	}
}

void ABlasterCharacter::ServerSetOverlappingWeapon_Implementation(AWeapon* pWeapon)
{
    if (m_pOverlappingWeapon)
    {
        m_pOverlappingWeapon->ShowPickupWidget(false);
    }
    
    m_pOverlappingWeapon = pWeapon;
    
    if (m_pOverlappingWeapon)
    {
        m_pOverlappingWeapon->ShowPickupWidget(true);
    }
}

void ABlasterCharacter::HideCameraWhenPlayerIsClose() const
{
	if (!IsLocallyControlled()) return;

	if (FVector::Dist(m_pFollowCamera->GetComponentLocation(), GetActorLocation()) < m_PlayerHideDistance)
	{
		GetMesh()->SetVisibility(false);
		if (m_pCombat->HasWeapon())
		{
			m_pCombat->m_pEquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
		if (m_pCombat->HasSecondaryWeapon())
			m_pCombat->m_pSecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = true;
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (m_pCombat->HasWeapon())
		{
			m_pCombat->m_pEquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
		if (m_pCombat->HasSecondaryWeapon())
			m_pCombat->m_pSecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = false;
	}
}

void ABlasterCharacter::OnRep_Health(float oldHealth)
{
	if (m_CurrentHealth < oldHealth) //Meaning we took damage
		PlayHitReactMontage();
	UpdateHudHealth();
}

void ABlasterCharacter::OnRep_Shield(float lastShieldValue)
{
	if (m_CurrentHealth < lastShieldValue) //Meaning we took damage
		PlayHitReactMontage();
	UpdateHudShield();
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
	
	//If we have two weapons and we are not overlapping any then we use this button to switch weapons
	if (m_pCombat->HasWeapon() && m_pCombat->HasSecondaryWeapon() && !m_pOverlappingWeapon)
		m_pCombat->SwapWeapons();
	
	m_pCombat->EquipWeapon(m_pOverlappingWeapon);
}

void ABlasterCharacter::ServerLeaveGame_Implementation()
{
	if (GetWorld()->GetAuthGameMode<ALobbyGameMode>())
	{
		OnLeftGame.Broadcast();
		return;
	}
	
	ABlasterGameMode* pGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	ABlasterPlayerState* pPlayerState = GetPlayerState<ABlasterPlayerState>();
	if (pGameMode && pPlayerState)
	{
		pGameMode->PlayerLeftGame(pPlayerState);
	}
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