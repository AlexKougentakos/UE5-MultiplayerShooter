// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/Character/BlasterCharacter.h"
#include "MultiplayerShooter/HUD/BlasterHUD.h"
#include "MultiplayerShooter/PlayerController/BlasterPlayerController.h"
#include "MultiplayerShooter/Weapon/Shotgun.h"
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
		m_pPlayerController->SetHudCarriedAmmo(m_CarriedAmmo);
	}

	m_pEquippedWeapon->UpdateHudAmmo();
}

void UCombatComponent::UpdateWeaponHUD()
{
	m_pPlayerController = m_pPlayerController == nullptr ? Cast<ABlasterPlayerController>(m_pCharacter->GetController()) : m_pPlayerController;
	if (m_pPlayerController)
	{
		m_pPlayerController->UpdateWeaponHud(m_pEquippedWeapon, m_pSecondaryWeapon);
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
	//Update the carried ammo if it's the same weapon type
	if (HasWeapon() && m_pEquippedWeapon->GetWeaponType() == weaponType)
		m_CarriedAmmo = m_CarriedAmmoMap[weaponType];
	
	UpdateAmmoHud();
	//Update the hud only when the equipped weapon is the same as the weapon type of the ammo you picked up
	//UpdateAmmoValues(HasWeapon() && m_pEquippedWeapon->GetWeaponType() == weaponType);

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
	if (m_CarriedAmmo <= 0 || m_CombatState == ECombatState::ECS_Reloading || m_pEquippedWeapon->IsMagazineFull() || m_IsLocallyReloading || !m_pCharacter->IsAlive()) return;

	SetAiming(false);
	ServerReload();
	HandleReloadingForBothServerAndClient();
	m_IsLocallyReloading = true;
}

void UCombatComponent::ServerReload_Implementation()
{
	checkf(m_pCharacter, TEXT("Character is nullptr"));

	m_CombatState = ECombatState::ECS_Reloading;
		
	if (!m_pCharacter->IsLocallyControlled()) HandleReloadingForBothServerAndClient();
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
		if (!m_pCharacter->IsLocallyControlled()) HandleReloadingForBothServerAndClient();
		break;
	case ECombatState::ECS_ThrowingGrenade:
		// The reason for the NOT is locally controlled is because the montage will have already been played on the local client
		// So we don't want to play it again
		if (m_pCharacter && !m_pCharacter->IsLocallyControlled())
		{
			m_pCharacter->PlayThrowGrenadeMontage();
		}
		break;
	case ECombatState::ECS_SwappingWeapons:
		if (!m_pCharacter->IsLocallyControlled()) //we already played the animation locally for the player to avoid delays
			m_pCharacter->PlayWeaponSwapMontage();
		break;
	}
}

void UCombatComponent::SetAiming(const bool isAiming)
{
	//If we are already out of the aiming state, we don't want to set it again
	if (m_IsAiming == isAiming) return;
	
	m_IsAiming = isAiming; //This is being set here to minimize delay on the local client for the events that require it
	ServerSetAiming(isAiming);
	checkf(m_pCharacter, TEXT("Character is nullptr"));
	
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeed = isAiming ? m_AimingWalkSpeed : m_BaseWalkSpeed;

	if (HasWeapon() && m_pCharacter->IsLocallyControlled() && m_pEquippedWeapon->GetWeaponType() == EWeaponType::EWT_Sniper)
	{
		m_pPlayerController = m_pPlayerController == nullptr ? Cast<ABlasterPlayerController>(m_pCharacter->GetController()) : m_pPlayerController;
		if (m_pPlayerController)
			m_pPlayerController->HideUI(isAiming);
		m_pCharacter->ShowSniperScopeWidget(isAiming);
	}

	if (m_pCharacter->IsLocallyControlled()) m_IsAimButtonPressed = isAiming;
}

void UCombatComponent::OnRep_Aiming()
{
	if (!m_pCharacter->IsLocallyControlled()) return;
	
	m_IsAiming = m_IsAimButtonPressed;	
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

	if (HasWeapon())
	{
		FireWeaponBasedOnFireType();
		m_CrosshairShootingFactor = 0.9f;
	}

	StartFireTimer();
}

void UCombatComponent::FireWeaponBasedOnFireType()
{
	switch (m_pEquippedWeapon->GetFireType())
	{
	case EFireType::EFT_HitScan:
		FireHitScanWeapon();
		break;
	case EFireType::EFT_Projectile:
		FireProjectileWeapon();
		break;
	case EFireType::EFT_Shotgun:
		FireShotgun();
		break;;
	}
}


void UCombatComponent::FireProjectileWeapon()
{
	m_HitTarget = m_pEquippedWeapon->UseScatter() ? m_pEquippedWeapon->GetVectorWithSpread(m_HitTarget, FMath::Max(0.f, m_HudPackage.CrosshairSpread)) : m_HitTarget;
	if(!m_pCharacter->HasAuthority()) LocalFire(m_HitTarget); //We don't want to shoot twice on the server
	ServerFire(m_HitTarget);
}

void UCombatComponent::FireHitScanWeapon()
{
	m_HitTarget = m_pEquippedWeapon->UseScatter() ? m_pEquippedWeapon->GetVectorWithSpread(m_HitTarget, FMath::Max(0.f, m_HudPackage.CrosshairSpread)) : m_HitTarget;
	if(!m_pCharacter->HasAuthority()) LocalFire(m_HitTarget); //We don't want to shoot twice on the server
	ServerFire(m_HitTarget);
}

void UCombatComponent::FireShotgun()
{
	TArray<FVector_NetQuantize> outShotLocations{};
	const AShotgun* pShotgun = Cast<AShotgun>(m_pEquippedWeapon);
	checkf(pShotgun, TEXT("Equipped weapon is not a shotgun"));
	
	pShotgun->ShotgunGetVectorWithSpread(m_HitTarget, outShotLocations, FMath::Max(0.f, m_HudPackage.CrosshairSpread) );
	if(!m_pCharacter->HasAuthority()) ShotgunLocalFire(outShotLocations); //We don't want to shoot twice on the server
	ShotgunServerFire(outShotLocations);
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

void UCombatComponent::ShotgunServerFire_Implementation(const TArray<FVector_NetQuantize>& traceHitLocations)
{
	MulticastFireShotgun(traceHitLocations);
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& traceHitLocation)
{
	MulticastFire(traceHitLocation);
}

void UCombatComponent::MulticastFireShotgun_Implementation(const TArray<FVector_NetQuantize>& traceHitLocations)
{
	checkf(m_pCharacter, TEXT("Character is nullptr"));
	if (m_pCharacter->IsLocallyControlled() && !m_pCharacter->HasAuthority()) return;
	
	ShotgunLocalFire(traceHitLocations);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& traceHitLocation)
{
	checkf(m_pCharacter, TEXT("Character is nullptr"));
	if (m_pCharacter->IsLocallyControlled() && !m_pCharacter->HasAuthority()) return;
	
	LocalFire(traceHitLocation);
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& traceHitLocation)
{
	if (!m_pEquippedWeapon || m_CombatState != ECombatState::ECS_Unoccupied) return;
    
	m_pCharacter->PlayFireMontage(m_IsAiming);
	m_pEquippedWeapon->Fire(traceHitLocation);
}

void UCombatComponent::ShotgunLocalFire(const TArray<FVector_NetQuantize>& traceHitLocations)
{
	if (!m_pEquippedWeapon || !m_pCharacter) return;
	if (m_CombatState == ECombatState::ECS_ThrowingGrenade) return;
	
	m_pCharacter->PlayFireMontage(m_IsAiming);

	AShotgun* pShotgun = Cast<AShotgun>(m_pEquippedWeapon);
	checkf(pShotgun, TEXT("Equipped weapon is not a shotgun"));
	pShotgun->FireShotgun(traceHitLocations);
	m_CombatState = ECombatState::ECS_Unoccupied;
	m_IsLocallyReloading = false;
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
	UpdateWeaponHUD();
}


void UCombatComponent::SwapWeapons()
{
	if (m_CombatState != ECombatState::ECS_Unoccupied || !m_pCharacter->HasAuthority()) return;
	
	m_pCharacter->PlayWeaponSwapMontage();
	m_CombatState = ECombatState::ECS_SwappingWeapons;

	m_pCharacter->SetFinishedSwappingWeapons(false);
	SetAiming(false);
}

bool UCombatComponent::ShouldSwapWeapons() const
{
	return HasWeapon() && HasSecondaryWeapon() && m_CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::EquipPrimaryWeapon(AWeapon* const pWeapon)
{
	m_pEquippedWeapon = pWeapon;
	
	if (m_CarriedAmmoMap.Contains(m_pEquippedWeapon->GetWeaponType()))
	{
		m_CarriedAmmo = m_CarriedAmmoMap[m_pEquippedWeapon->GetWeaponType()];
	}
	
	m_pEquippedWeapon->SetOwner(m_pCharacter);
	m_pEquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		
	UpdateAmmoHud();

	const USkeletalMeshSocket* weaponSocket =  m_pCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (weaponSocket) //This has to be after the set weapon state
		weaponSocket->AttachActor(m_pEquippedWeapon, m_pCharacter->GetMesh());

	
	checkf(m_pEquippedWeapon->GetPickupSound(), TEXT("Pickup sound is nullptr"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pEquippedWeapon->GetPickupSound(), m_pCharacter->GetActorLocation());

	if (!m_pEquippedWeapon->HasAmmoInMagazine())
	{
		Reload();
	}	
}

void UCombatComponent::EquipSecondaryWeapon(AWeapon* const pWeapon)
{
	m_pSecondaryWeapon = pWeapon;
	m_pSecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	m_pSecondaryWeapon->SetOwner(m_pCharacter);

	//This has to be after the set weapon state
	AttachActorToBackpack(pWeapon);
	
	checkf(m_pSecondaryWeapon->GetPickupSound(), TEXT("Pickup sound is nullptr"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pSecondaryWeapon->GetPickupSound(), m_pCharacter->GetActorLocation());
	
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
	
	//I repeat this here because we have two things that are replicated, the weapon state and the attachment of the weapon to the character
	// Just because I called one before the other doesn't guarantee that it will happen first so there's a chance that the weapon will be attached to the character before the weapon state is set
	// meaning that the weapon will still have physics enabled when you pick it up, which is why I am explicitly setting it here
	m_pEquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	
	const USkeletalMeshSocket* weaponSocket =  m_pCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (weaponSocket)
		weaponSocket->AttachActor(m_pEquippedWeapon, m_pCharacter->GetMesh());
	
	m_pCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	m_pCharacter->bUseControllerRotationYaw = true;
	
	checkf(m_pEquippedWeapon->GetPickupSound(), TEXT("Pickup sound is nullptr"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pEquippedWeapon->GetPickupSound(), m_pCharacter->GetActorLocation());

	UpdateAmmoHud();
	UpdateAmmoValues(true);
	UpdateWeaponHUD();
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
	UpdateWeaponHUD();
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
		m_pHud->SetHudPackage(m_HudPackage);
		return;
	}
	
	// Calculate the spread
	const FVector2D walkSpeedRange(0.f, m_pCharacter->GetCharacterMovement()->MaxWalkSpeed);
	const FVector2D velocityMultiplierRange{m_pEquippedWeapon->GetMovingFactorMin(), m_pEquippedWeapon->GetMovingFactorMax()};
	FVector velocity = m_pCharacter->GetVelocity();
	velocity.Z = 0.f;
		
	float crosshairVelocityFactor =  FMath::GetMappedRangeValueClamped(walkSpeedRange, velocityMultiplierRange, velocity.Size());
	//if the player is not actually moving then we won't apply the velocity factor
	if (velocity.Size() < 0.5f)
		crosshairVelocityFactor = 0.f;
	
	if (m_pCharacter->GetMovementComponent()->IsFalling())
		m_CrosshairInAirFactor = FMath::FInterpTo(m_CrosshairInAirFactor, m_pEquippedWeapon->GetInAirFactor(), deltaTime, 2.25f);
	else
		m_CrosshairInAirFactor = FMath::FInterpTo(m_CrosshairInAirFactor, 0.f, deltaTime, 30.f);

	if (m_IsAiming)
		m_CrosshairAimFactor = FMath::FInterpTo(m_CrosshairAimFactor, m_pEquippedWeapon->GetAimFactor(), deltaTime, 30.f);
	else
		m_CrosshairAimFactor = FMath::FInterpTo(m_CrosshairAimFactor, 0.f, deltaTime, 30.f);
	
	if (m_pCharacter->GetMovementComponent()->IsCrouching())
		m_CrosshairCrouchingFactor = FMath::FInterpTo(m_CrosshairCrouchingFactor, m_pEquippedWeapon->GetCrouchingFactor(), deltaTime, 30.f);
	else
		m_CrosshairCrouchingFactor = FMath::FInterpTo(m_CrosshairCrouchingFactor, 0.f, deltaTime, 30.f);
		

	m_CrosshairShootingFactor = FMath::FInterpTo(m_CrosshairShootingFactor, m_pEquippedWeapon->GetShootingFactor(), deltaTime, 30.f);
	
	//Add all the different spread factors together
	m_HudPackage.CrosshairSpread =
		m_pEquippedWeapon->GetBaseSpread() + // Base spread
		crosshairVelocityFactor + // How fast you are moving
		m_CrosshairShootingFactor + // If you are shooting
		m_CrosshairInAirFactor +  //if you are in the air
		m_CrosshairAimFactor + //if you are aiming
		m_CrosshairCrouchingFactor; //if you are crouching
	
	m_pHud->SetHudPackage(m_HudPackage);
}

bool UCombatComponent::CanFire() const
{
	if (!HasWeapon()) return false;
	//Special exception where you can fire the shotgun if you are reloading
	if (m_CanFire &&
		m_pEquippedWeapon->HasAmmoInMagazine() &&
		m_CombatState == ECombatState::ECS_Reloading &&
		m_pEquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun) return true;

	if (m_IsLocallyReloading) return false;
	
	return m_CanFire && m_IsFireButtonPressed && m_pEquippedWeapon->HasAmmoInMagazine() && m_CombatState == ECombatState::ECS_Unoccupied; 
	
}

void UCombatComponent::FinishedReloading()
{
	checkf(m_pCharacter, TEXT("Character is nullptr"));
	m_IsLocallyReloading = false;
	if (m_pCharacter->HasAuthority())
	{
		m_CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues(true);
	}

	if (m_IsFireButtonPressed) Fire();
}

void UCombatComponent::FinishedWeaponSwap()
{
	if (m_pCharacter->HasAuthority())
	{
		m_CombatState = ECombatState::ECS_Unoccupied;
	}

	m_pCharacter->SetFinishedSwappingWeapons(true);
}

void UCombatComponent::FinishedWeaponSwapAttachment()
{
	if (!m_pCharacter || !m_pCharacter->HasAuthority()) return;
	
	AWeapon* pTempWeapon = m_pEquippedWeapon;
	m_pEquippedWeapon = m_pSecondaryWeapon;
	m_pSecondaryWeapon = pTempWeapon;
	UpdateWeaponHUD();
	
	if (m_CarriedAmmoMap.Contains(m_pEquippedWeapon->GetWeaponType()))
	{
		m_CarriedAmmo = m_CarriedAmmoMap[m_pEquippedWeapon->GetWeaponType()];
	}
	
	/*
	 *	PRIMARY WEAPON 
	 */
	UpdateAmmoHud();
	

	m_pEquippedWeapon->SetOwner(m_pCharacter);
	m_pEquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	
	
	const USkeletalMeshSocket* weaponSocket =  m_pCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (weaponSocket)
		weaponSocket->AttachActor(m_pEquippedWeapon, m_pCharacter->GetMesh());

	UE_LOG(LogTemp, Warning, TEXT("Sound"));
	checkf(m_pEquippedWeapon->GetPickupSound(), TEXT("Pickup sound is nullptr"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pEquippedWeapon->GetPickupSound(), m_pCharacter->GetActorLocation());

	if (!m_pEquippedWeapon->HasAmmoInMagazine())
	{
		Reload();
	}

	/*
	 *	SECONDARY WEAPON 
	 */
	m_pSecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(m_pSecondaryWeapon);
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