﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/HUD/BlasterHUD.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"
#include "MultiplayerShooter/Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
	m_AimingWalkSpeed = 450.0f;
}


void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, m_pEquippedWeapon);
	DOREPLIFETIME(ThisClass, m_pSecondaryWeapon);
	DOREPLIFETIME(ThisClass, m_IsAiming);
	DOREPLIFETIME(ThisClass, m_CombatState);
	DOREPLIFETIME_CONDITION(ThisClass, m_CarriedAmmo, COND_OwnerOnly);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(m_pCharacter, TEXT("Character is nullptr"));
	m_BaseWalkSpeed = m_pCharacter->GetCharacterMovement()->MaxWalkSpeed;
	
	const auto pPlayerCamera = m_pCharacter->GetFollowCamera();
	checkf(pPlayerCamera, TEXT("Player camera is nullptr"));
	
	m_DefaultFOV = pPlayerCamera->FieldOfView;
	m_CurrentFOV = m_DefaultFOV;
}

void UCombatComponent::TickComponent(float deltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(deltaTime, TickType, ThisTickFunction);


	if (m_pCharacter->IsLocallyControlled())
	{
		FHitResult hitResult{};
		TraceUnderCrosshairs(hitResult);
		m_HitTarget = hitResult.ImpactPoint;
		SetHudCrosshairs(deltaTime);

		// Handles the FOV changes when aiming
		InterpolateFOV(deltaTime);
	}
	
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& hitResult)
{
	FVector2D viewport{};
	GEngine->GameViewport->GetViewportSize(viewport);

	const FVector2D crosshairLocation = viewport / 2.0f;
	FVector crosshairWorldPosition{};
	FVector crosshairWorldDirection{};

	const bool screenToWorldSucceeded =
		UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(GetWorld(), 0), crosshairLocation, crosshairWorldPosition, crosshairWorldDirection);

	if (!screenToWorldSucceeded) return;

	FVector start = crosshairWorldPosition;

	checkf(m_pCharacter, TEXT("Character is nullptr"));
	const float distanceToCharacter = FVector::Dist(start, m_pCharacter->GetActorLocation());
	start += crosshairWorldDirection * (distanceToCharacter + 100.f);
	
	
	const FVector end = start + crosshairWorldDirection * BULLET_TRACE_LENGTH;
	GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility);

	// Detect when you are aiming at a player
	if (hitResult.GetActor() && hitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		m_HudPackage.CrosshairColor = FLinearColor::Red;
	else
		m_HudPackage.CrosshairColor = FLinearColor::White;

	// Hide crosshairs when inside a sniper scope
	if (HasWeapon() && m_pEquippedWeapon->GetWeaponType() == EWeaponType::EWT_Sniper && m_IsAiming)
	{
		m_HudPackage.CrosshairColor = FLinearColor::Transparent;
	}
	
	// Manually set the end point to the furthest away point when the trace doesn't hit anything (example: when shooting in the sky)
	if (!hitResult.bBlockingHit)
	{
		hitResult.ImpactPoint = end;
	}
	 
}


void UCombatComponent::UpdateAmmoValues(bool refreshHud)
{
	if (!HasWeapon()) return;
	checkf(m_CarriedAmmoMap.Contains(m_pEquippedWeapon->GetWeaponType()), TEXT("Carried ammo map does not contain the weapon type"));

	m_CarriedAmmo -= m_pEquippedWeapon->Reload(m_CarriedAmmo);
	//Update the values on the map as well
	m_CarriedAmmoMap[m_pEquippedWeapon->GetWeaponType()] = m_CarriedAmmo;
	
	if (m_pCharacter->HasAuthority() && refreshHud)
		OnRep_CarriedAmmo(); //This is just to update the HUD for the server, you can refactor it but I'm lazy
}

void UCombatComponent::UpdateShotgunAmmoValues()
{
	if (!HasWeapon()) return;
	checkf(m_CarriedAmmoMap.Contains(m_pEquippedWeapon->GetWeaponType()), TEXT("Carried ammo map does not contain the weapon type"));

	const int reloadedAmount = m_pEquippedWeapon->Reload(1);
	m_CarriedAmmo -= reloadedAmount;

	m_CanFire = true;
	
	if (m_pEquippedWeapon->IsMagazineFull() || m_CarriedAmmo <= 0)
	{
		JumpToShotgunReloadAnimationEnd();
	}
}

void UCombatComponent::UpdateAmmoHud()
{
	m_pPlayerController = m_pPlayerController == nullptr ? Cast<ABlasterPlayerController>(m_pCharacter->GetController()) : m_pPlayerController;
	if (m_pPlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Updating HUD Ammo"));
		m_pPlayerController->SetHudCarriedAmmo(m_CarriedAmmo);
		m_pPlayerController->ShowAmmo(true);
	}
}

void UCombatComponent::JumpToShotgunReloadAnimationEnd() const
{
	const auto pAnimInstance = Cast<ABlasterCharacter>(m_pCharacter)->GetMesh()->GetAnimInstance();
	if (pAnimInstance)
		pAnimInstance->Montage_JumpToSection("ShotgunEND");
}

void UCombatComponent::ThrowGrenadeFinished()
{
	m_CombatState = ECombatState::ECS_Unoccupied;
}

void UCombatComponent::PickupAmmo(const EWeaponType weaponType, const int ammoAmount)
{
	checkf(m_CarriedAmmoMap.Contains(weaponType), TEXT("Carried ammo map does not contain the weapon type"));

	m_CarriedAmmoMap[weaponType] += ammoAmount;
	m_CarriedAmmo = m_CarriedAmmoMap[weaponType];

	//Update the hud only when the equipped weapon is the same as the weapon type of the ammo you picked up
	UpdateAmmoValues(HasWeapon() && m_pEquippedWeapon->GetWeaponType() == weaponType);

	if (HasWeapon() &&
		m_pEquippedWeapon->GetWeaponType() == weaponType &&
		!m_pEquippedWeapon->HasAmmoInMagazine() &&
		m_CombatState == ECombatState::ECS_Unoccupied)
	{
		Reload();
	}
}

void UCombatComponent::Reload()
{
	if (m_CarriedAmmo <= 0 || m_CombatState == ECombatState::ECS_Reloading || m_pEquippedWeapon->IsMagazineFull()) return;
	
	ServerReload();
}

void UCombatComponent::ServerReload_Implementation()
{
	checkf(m_pCharacter, TEXT("Character is nullptr"));

	m_CombatState = ECombatState::ECS_Reloading;
		
	HandleReloadingForBothServerAndClient();
}

void UCombatComponent::HandleReloadingForBothServerAndClient()
{
	m_pCharacter->PlayRifleReloadMontage();
}

void UCombatComponent::OnRep_CombatState()
{
	switch (m_CombatState)
	{
	case ECombatState::ECS_Unoccupied:
		if (m_IsFireButtonPressed) Fire();
		break;
	case ECombatState::ECS_Reloading:
		HandleReloadingForBothServerAndClient();
		break;
	case ECombatState::ECS_ThrowingGrenade:
		// The reason for the NOT is locally controlled is because the montage will have already been played on the local client
		// So we don't want to play it again
		if (m_pCharacter && !m_pCharacter->IsLocallyControlled())
		{
			m_pCharacter->PlayThrowGrenadeMontage();
		}
		break;
	case ECombatState::ECS_MAX:
		break;
	}
}


void UCombatComponent::SetAiming(const bool isAiming)
{
	m_IsAiming = isAiming; //This is being set here to minimize delay on the local client for the events that require it
	ServerSetAiming(isAiming);
	checkf(m_pCharacter, TEXT("Character is nullptr"));
	
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeed = isAiming ? m_AimingWalkSpeed : m_BaseWalkSpeed;

	if (HasWeapon() && m_pCharacter->IsLocallyControlled() && m_pEquippedWeapon->GetWeaponType() == EWeaponType::EWT_Sniper)
		m_pCharacter->ShowSniperScopeWidget(isAiming);
}

void UCombatComponent::FireButtonPressed(const bool isPressed)
{
	m_IsFireButtonPressed = isPressed;

	if (isPressed)
	{
		Fire();
	}
}

void UCombatComponent::ShotgunShellReload()
{
	if (m_pCharacter && m_pCharacter->HasAuthority())
		UpdateShotgunAmmoValues();
}

void UCombatComponent::Fire()
{
	if (!CanFire()) return;

	m_CanFire = false;
	ServerFire(m_HitTarget);

	if (HasWeapon())
		m_CrosshairShootingFactor = 0.9f;

	StartFireTimer();
}

void UCombatComponent::StartFireTimer()
{
	if (!HasWeapon())
	{
		m_CanFire = true;
		return;	
	}
	
	m_pCharacter->GetWorldTimerManager().SetTimer(m_FireTimer, this, &UCombatComponent::FireTimerFinished, m_pEquippedWeapon->GetFireDelay());
	
}

void UCombatComponent::FireTimerFinished()
{
	m_CanFire = true;
	if (m_IsFireButtonPressed && m_pEquippedWeapon->HasAutomaticFire())
	{
		Fire();
	}

	if (!m_pEquippedWeapon->HasAmmoInMagazine())
	{
		Reload();
	}
	
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	m_pPlayerController = m_pPlayerController == nullptr ? Cast<ABlasterPlayerController>(m_pCharacter->GetController()) : m_pPlayerController;
	if (m_pPlayerController)
	{
		m_pPlayerController->SetHudCarriedAmmo(m_CarriedAmmo);
	}

	const bool jumpToShotgunReloadAnimationEnd = HasWeapon() &&
		m_CombatState == ECombatState::ECS_Reloading &&
			m_pEquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun
	&& m_CarriedAmmo <= 0;

	if (jumpToShotgunReloadAnimationEnd) JumpToShotgunReloadAnimationEnd();
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& traceHitLocation)
{
	MulticastFire(traceHitLocation);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& traceHitLocation)
{
	checkf(m_pCharacter, TEXT("Character is nullptr"));

	if (m_pEquippedWeapon && m_CombatState == ECombatState::ECS_Reloading && m_pEquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun)
	{
		m_pCharacter->PlayFireMontage(m_IsAiming);
		m_pEquippedWeapon->Fire(traceHitLocation);
		m_CombatState = ECombatState::ECS_Unoccupied;
		return;
	}
	
    if (!m_pEquippedWeapon || m_CombatState != ECombatState::ECS_Unoccupied) return;
    
	m_pCharacter->PlayFireMontage(m_IsAiming);
	m_pEquippedWeapon->Fire(traceHitLocation);
}

void UCombatComponent::ServerSetAiming_Implementation(const bool isAiming)
{
	m_IsAiming = isAiming;
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeed = isAiming ? m_AimingWalkSpeed : m_BaseWalkSpeed;
}


void UCombatComponent::EquipWeapon(AWeapon* const pWeapon)
{
	if (!m_pCharacter || !pWeapon) return;
	if (m_CombatState != ECombatState::ECS_Unoccupied) return;
	if (HasWeapon() && HasSecondaryWeapon())
	{
		if (m_pEquippedWeapon->ShouldDestroyWeapon())
			m_pEquippedWeapon->Destroy();
		else
			m_pEquippedWeapon->Drop();
	}

	if (HasWeapon() && !HasSecondaryWeapon())
	{
		if (m_pEquippedWeapon->ShouldDestroyWeapon())
		{
			m_pEquippedWeapon->Destroy();
			EquipPrimaryWeapon(pWeapon);
		}
		else EquipSecondaryWeapon(pWeapon);
	}
	else
		EquipPrimaryWeapon(pWeapon);

	
	m_pCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	m_pCharacter->bUseControllerRotationYaw = true;
}


void UCombatComponent::SwapWeapons()
{
	AWeapon* pTempWeapon = m_pEquippedWeapon;
	m_pEquippedWeapon = m_pSecondaryWeapon;
	m_pSecondaryWeapon = pTempWeapon;

	if (m_CarriedAmmoMap.Contains(m_pEquippedWeapon->GetWeaponType()))
	{
		m_CarriedAmmo = m_CarriedAmmoMap[m_pEquippedWeapon->GetWeaponType()];
	}
	
	/*
	 *	PRIMARY WEAPON 
	 */
	m_pEquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	
	UpdateAmmoHud();

	const USkeletalMeshSocket* weaponSocket =  m_pCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (weaponSocket)
		weaponSocket->AttachActor(m_pEquippedWeapon, m_pCharacter->GetMesh());

	checkf(m_pEquippedWeapon->GetPickupSound(), TEXT("Pickup sound is nullptr"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pEquippedWeapon->GetPickupSound(), m_pCharacter->GetActorLocation());

	if (!m_pEquippedWeapon->HasAmmoInMagazine())
	{
		Reload();
	}
	
	m_pEquippedWeapon->SetOwner(m_pCharacter);
	m_pEquippedWeapon->UpdateHudAmmo();

	/*
	 *	SECONDARY WEAPON 
	 */
	m_pSecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(m_pSecondaryWeapon);
}

void UCombatComponent::EquipPrimaryWeapon(AWeapon* const pWeapon)
{
		
	m_pEquippedWeapon = pWeapon;
	m_pEquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	
	if (m_CarriedAmmoMap.Contains(m_pEquippedWeapon->GetWeaponType()))
	{
		m_CarriedAmmo = m_CarriedAmmoMap[m_pEquippedWeapon->GetWeaponType()];
	}
		
	UpdateAmmoHud();

	const USkeletalMeshSocket* weaponSocket =  m_pCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (weaponSocket)
		weaponSocket->AttachActor(m_pEquippedWeapon, m_pCharacter->GetMesh());

	
	checkf(m_pEquippedWeapon->GetPickupSound(), TEXT("Pickup sound is nullptr"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pEquippedWeapon->GetPickupSound(), m_pCharacter->GetActorLocation());

	if (!m_pEquippedWeapon->HasAmmoInMagazine())
	{
		Reload();
	}
	
	m_pEquippedWeapon->SetOwner(m_pCharacter);
	m_pEquippedWeapon->UpdateHudAmmo();
}

void UCombatComponent::EquipSecondaryWeapon(AWeapon* const pWeapon)
{
	m_pSecondaryWeapon = pWeapon;
	m_pSecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(pWeapon);
	
	checkf(m_pSecondaryWeapon->GetPickupSound(), TEXT("Pickup sound is nullptr"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pSecondaryWeapon->GetPickupSound(), m_pCharacter->GetActorLocation());
	
	m_pSecondaryWeapon->SetOwner(m_pCharacter);
}

void UCombatComponent::AttachActorToBackpack(AActor* const pActor)
{
	if (!m_pCharacter || !pActor || !m_pCharacter->GetMesh()) return;

	const USkeletalMeshSocket* pBackpackSocket = m_pCharacter->GetMesh()->GetSocketByName(FName("BackpackSocket"));
	if (!pBackpackSocket) return;

	pBackpackSocket->AttachActor(pActor, m_pCharacter->GetMesh());
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (!m_pEquippedWeapon) return;
	UE_LOG(LogTemp, Warning, TEXT("Carried Ammo: %d"), m_CarriedAmmo);

	//I repeat this here because we have two things that are replicated, the weapon state and the attachment of the weapon to the character
	// Just because I called one before the other doesn't guarantee that it will happen first so there's a chance that the weapon will be attached to the character before the weapon state is set
	// meaning that the weapon will still have physics enabled when you pick it up, which is why I am explicitly setting it here
	m_pEquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* weaponSocket =  m_pCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (weaponSocket)
		weaponSocket->AttachActor(m_pEquippedWeapon, m_pCharacter->GetMesh());
	
	m_pCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	m_pCharacter->bUseControllerRotationYaw = true;

	m_pPlayerController = m_pPlayerController == nullptr ? Cast<ABlasterPlayerController>(m_pCharacter->GetController()) : m_pPlayerController;
	if (m_pPlayerController)
	{
		m_pPlayerController->ShowAmmo(true);
	}

	checkf(m_pEquippedWeapon->GetPickupSound(), TEXT("Pickup sound is nullptr"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pEquippedWeapon->GetPickupSound(), m_pCharacter->GetActorLocation());

	UpdateAmmoHud();
	m_pEquippedWeapon->UpdateHudAmmo();
	UpdateAmmoValues(true);
}

void UCombatComponent::OnRep_SecondaryWeapon()
{
	if (m_pSecondaryWeapon)
	{
		m_pSecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
		AttachActorToBackpack(m_pSecondaryWeapon);
		
		checkf(m_pSecondaryWeapon->GetPickupSound(), TEXT("Pickup sound is nullptr"));
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pSecondaryWeapon->GetPickupSound(), m_pCharacter->GetActorLocation());
	}
}

void UCombatComponent::SetHudCrosshairs(float deltaTime)
{
	checkf(m_pCharacter, TEXT("Character is nullptr"));

	m_pPlayerController = m_pPlayerController == nullptr ? Cast<ABlasterPlayerController>(m_pCharacter->GetController()) : m_pPlayerController;
	if (!m_pPlayerController) return;

	m_pHud = m_pHud == nullptr ? Cast<ABlasterHUD>(m_pPlayerController->GetHUD()) : m_pHud;
	if(!m_pHud) return;
	if (HasWeapon())
	{
		m_HudPackage.CrosshairsCenter = m_pEquippedWeapon->m_pCrosshairsCenter;
		m_HudPackage.CrosshairsTop = m_pEquippedWeapon->m_pCrosshairsTop;
		m_HudPackage.CrosshairsRight = m_pEquippedWeapon->m_pCrosshairsRight;
		m_HudPackage.CrosshairsBottom = m_pEquippedWeapon->m_pCrosshairsBottom;
		m_HudPackage.CrosshairsLeft = m_pEquippedWeapon->m_pCrosshairsLeft;	
	}
	else
	{
		m_HudPackage.CrosshairsCenter = nullptr;
		m_HudPackage.CrosshairsTop = nullptr;
		m_HudPackage.CrosshairsRight = nullptr;
		m_HudPackage.CrosshairsBottom = nullptr;
		m_HudPackage.CrosshairsLeft = nullptr;
	}
	
	// Calculate the spread
	const FVector2D walkSpeedRange(0.f, m_pCharacter->GetCharacterMovement()->MaxWalkSpeed);
	const FVector2D velocityMultiplierRange{0.f, 1.f};
	FVector velocity = m_pCharacter->GetVelocity();
	velocity.Z = 0.f;
	
	const float crosshairVelocityFactor =  FMath::GetMappedRangeValueClamped(walkSpeedRange, velocityMultiplierRange, velocity.Size());

	if (m_pCharacter->GetMovementComponent()->IsFalling())
		m_CrosshairInAirFactor = FMath::FInterpTo(m_CrosshairInAirFactor, 2.25f, deltaTime, 2.25f);
	else
		m_CrosshairInAirFactor = FMath::FInterpTo(m_CrosshairInAirFactor, 0.f, deltaTime, 30.f);

	if (m_IsAiming)
		m_CrosshairAimFactor = FMath::FInterpTo(m_CrosshairAimFactor, 0.5f, deltaTime, 30.f);
	else
		m_CrosshairAimFactor = FMath::FInterpTo(m_CrosshairAimFactor, 0.f, deltaTime, 30.f);

	m_CrosshairShootingFactor = FMath::FInterpTo(m_CrosshairShootingFactor, 0.f, deltaTime, 30.f);

	//Add all the different spread factors together
	m_HudPackage.CrosshairSpread =
		0.5f + 						// Base spread
		crosshairVelocityFactor + // How fast you are moving
		m_CrosshairShootingFactor + // If you are shooting
		m_CrosshairInAirFactor -  //if you are in the air
		m_CrosshairAimFactor; //if you are aiming
	
	m_pHud->SetHudPackage(m_HudPackage);
	
}

bool UCombatComponent::CanFire() const
{
	if (!HasWeapon()) return false;
	return m_CanFire &&
		m_IsFireButtonPressed &&
			m_pEquippedWeapon->HasAmmoInMagazine()
	//Special exception where you can fire the shotgun if you are reloading
	&& (m_CombatState == ECombatState::ECS_Unoccupied || (m_CombatState == ECombatState::ECS_Reloading && m_pEquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun));
}

void UCombatComponent::FinishedReloading()
{
	checkf(m_pCharacter, TEXT("Character is nullptr"));
	
	if (m_pCharacter->HasAuthority())
	{
		m_CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues(true);
	}

	if (m_IsFireButtonPressed) Fire();
		
}

void UCombatComponent::ThrowGrenade()
{
	if (m_CombatState != ECombatState::ECS_Unoccupied) return;
	
	m_CombatState = ECombatState::ECS_ThrowingGrenade;

	checkf (m_pCharacter, TEXT("Character is nullptr"));
	m_pCharacter->PlayThrowGrenadeMontage();

	if (!m_pCharacter->HasAuthority())
		ServerThrowGranade();
}

void UCombatComponent::ServerThrowGranade_Implementation()
{
	m_CombatState = ECombatState::ECS_ThrowingGrenade;

	checkf (m_pCharacter, TEXT("Character is nullptr"));
	m_pCharacter->PlayThrowGrenadeMontage();
}

void UCombatComponent::InterpolateFOV(const float deltaTime)
{
	if (!HasWeapon()) return;

	if (m_IsAiming)
		m_CurrentFOV = FMath::FInterpTo(m_CurrentFOV, m_pEquippedWeapon->GetZoomedFOV(), deltaTime, m_pEquippedWeapon->GetZoomInterpolationSpeed()); //Zooming in is defined by the weapon
	else
		m_CurrentFOV = FMath::FInterpTo(m_CurrentFOV, m_DefaultFOV, deltaTime, m_ZoomInterpolationSpeed); //Zooming out is always the same speed

	m_pCharacter->GetFollowCamera()->SetFieldOfView(m_CurrentFOV);
}

