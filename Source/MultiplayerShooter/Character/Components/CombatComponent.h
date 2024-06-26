// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABlasterCharacter;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* const pWeapon);
	bool HasWeapon() const { return m_pEquippedWeapon != nullptr; }
protected:
	virtual void BeginPlay() override;
	void SetAiming(const bool isAiming);
	void FireButtonPressed(const bool isPressed);

	void TraceUnderCrosshairs(FHitResult& hitResult);
	
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(const bool isAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& traceHitLocation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& traceHitLocation);

private:
	ABlasterCharacter* m_pCharacter{};
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* m_pEquippedWeapon{};

	UPROPERTY(Replicated)
	bool m_IsAiming{};

	bool m_IsFireButtonPressed{};

	UPROPERTY(EditAnywhere, Category = Movement, DisplayName = "Base Walk Speed")
	float m_BaseWalkSpeed{};

	UPROPERTY(EditAnywhere, Category = Movement, DisplayName = "Aiming Walk Speed")
	float m_AimingWalkSpeed{};

	UPROPERTY(Replicated)
	FVector m_HitTarget{};
};
