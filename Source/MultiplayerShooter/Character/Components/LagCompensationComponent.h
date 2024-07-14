#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"


class ABlasterPlayerController;

USTRUCT()
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location{};
	UPROPERTY()
	FRotator Rotation{};
	UPROPERTY()
	FVector BoxExtend{};
};

USTRUCT()
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time{};

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo{};
};

USTRUCT()
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool WasHitConfirmed{};
	UPROPERTY()
	bool Headshot{};
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYERSHOOTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULagCompensationComponent();
	friend class ABlasterCharacter;

	virtual void BeginPlay() override;
	void UpdateFrameHistory();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ShowFramePackage(const FFramePackage& framePackage, FColor color);
	FServerSideRewindResult ServerSideRewind(ABlasterCharacter* pHitCharacter, const FVector_NetQuantize& traceStart, const FVector_NetQuantize& traceEnd, const float hitTime);

	UFUNCTION(Server, Reliable)
	void ServerDamageRequest(ABlasterCharacter* pHitCharacter, const FVector_NetQuantize& traceStart, const FVector_NetQuantize& traceEnd, const float hitTime, AWeapon* pDamageCauser);
private:
	ABlasterCharacter* m_pCharacter{};
	ABlasterPlayerController* m_pBlasterPlayerController{};

	UPROPERTY(EditAnywhere, Category = "Lag Compensation", DisplayName = "Max Recording Time")
	float m_MaxRecordingTime{4.f};
	
	TDoubleLinkedList<FFramePackage> m_FrameHistory{};
	void SaveFramePackage(FFramePackage& framePackage);
	FFramePackage InterpolateBetweenFrames(const FFramePackage& olderFrame, const FFramePackage& youngerFrame, const float hitTime);
	FServerSideRewindResult ConfirmHit(const FFramePackage& framePackage, ABlasterCharacter* pHitCharacter, const FVector_NetQuantize& traceStart, const FVector_NetQuantize& traceEnd);
	void GetPlayerHitBoxes(ABlasterCharacter* pHitCharacter, FFramePackage& outFramePackage);
	void MovePlayerHitBoxes(ABlasterCharacter* pHitCharacter, const FFramePackage& outFramePackage);
	void ResetPlayerHitBoxes(ABlasterCharacter* pHitCharacter, const FFramePackage& package);
};
