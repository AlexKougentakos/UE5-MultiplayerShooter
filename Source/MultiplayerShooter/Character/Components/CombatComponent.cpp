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

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
	m_AimingWalkSpeed = 450.0f;
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

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, m_pEquippedWeapon);
	DOREPLIFETIME(ThisClass, m_IsAiming);
}

void UCombatComponent::SetAiming(const bool isAiming)
{
	m_IsAiming = isAiming; //This is being set here to minimize delay on the local client for the events that require it
	ServerSetAiming(isAiming);
	checkf(m_pCharacter, TEXT("Character is nullptr"));
	
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeed = isAiming ? m_AimingWalkSpeed : m_BaseWalkSpeed;
}

void UCombatComponent::FireButtonPressed(const bool isPressed)
{
	m_IsFireButtonPressed = isPressed;

	if (isPressed)
	{
		
		FHitResult hitResult{};
		TraceUnderCrosshairs(hitResult);
		ServerFire(hitResult.ImpactPoint);

		if (!HasWeapon()) return;

		m_CrosshairShootingFactor = 0.9f;
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

	const FVector start = crosshairWorldPosition;
	const FVector end = start + crosshairWorldDirection * 100000.0f;
	GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility);

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

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& traceHitLocation)
{
	MulticastFire(traceHitLocation);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& traceHitLocation)
{
	checkf(m_pCharacter, TEXT("Character is nullptr"));

    if (!m_pEquippedWeapon) return;
    
    m_pCharacter->PlayFireMontage(m_IsAiming);
    m_pEquippedWeapon->Fire(traceHitLocation);
}

void UCombatComponent::ServerSetAiming_Implementation(const bool isAiming)
{
	m_IsAiming = isAiming;
	m_pCharacter->GetCharacterMovement()->MaxWalkSpeed = isAiming ? m_AimingWalkSpeed : m_BaseWalkSpeed;
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (!m_pEquippedWeapon) return;
	
	m_pCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	m_pCharacter->bUseControllerRotationYaw = true;
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

void UCombatComponent::InterpolateFOV(const float deltaTime)
{
	if (!HasWeapon()) return;

	if (m_IsAiming)
		m_CurrentFOV = FMath::FInterpTo(m_CurrentFOV, m_pEquippedWeapon->GetZoomedFOV(), deltaTime, m_pEquippedWeapon->GetZoomInterpolationSpeed()); //Zooming in is defined by the weapon
	else
		m_CurrentFOV = FMath::FInterpTo(m_CurrentFOV, m_DefaultFOV, deltaTime, m_ZoomInterpolationSpeed); //Zooming out is always the same speed

	m_pCharacter->GetFollowCamera()->SetFieldOfView(m_CurrentFOV);
}

void UCombatComponent::EquipWeapon(AWeapon* const pWeapon)
{
	if (!m_pCharacter || !pWeapon) return;

	m_pEquippedWeapon = pWeapon;
	m_pEquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* weaponSocket =  m_pCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (!weaponSocket) return;
	weaponSocket->AttachActor(m_pEquippedWeapon, m_pCharacter->GetMesh());
	
	m_pEquippedWeapon->SetOwner(m_pCharacter);
	m_pCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	m_pCharacter->bUseControllerRotationYaw = true;

	
}
