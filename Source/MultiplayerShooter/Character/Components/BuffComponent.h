// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


class ABlasterCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYERSHOOTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuffComponent();
	friend class ABlasterCharacter;

	virtual void BeginPlay() override;
	virtual void TickComponent(float deltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Heal(const float amount, const float time);
	void AddShield(const float amount, const float time);
	void BuffSpeed(const float baseSpeed, const float crouchedSpeed, const float time);
	void BuffJump(const float jumpVelocity, const float time);
private:
	ABlasterCharacter* m_pCharacter{};

	/*
	 * HEALTH BUFF 
	 */
	bool m_IsHealing{ false };
	float m_AmountToHeal{ 0.f };
	float m_HealingRate{ 0.f };
	
	void UpdateHealing(float deltaTime);

	/*
	 * SHIELD BUFF 
	 */
	bool m_IsApplyingShield{false};
	float m_AmountToApplyShield{0.f};
	float m_ShieldRate{0.f};

	void UpdateShield(float deltaTime);
	
	/*
	 * SPEED BUFF
	 */

	float m_InitialBaseSpeed{};
	float m_InitialCrouchedSpeed{};
	
	FTimerHandle m_SpeedBuffTimerHandle{};
	void ResetSpeed();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float baseSpeed, float crouchSpeed);

	/*
	 * JUMP BUFF
	 */

	FTimerHandle m_JumpBuffTimerHandle{};
	void ResetJump();

	float m_InitialJumpVelocity{};
	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float jumpVelocity);
};
