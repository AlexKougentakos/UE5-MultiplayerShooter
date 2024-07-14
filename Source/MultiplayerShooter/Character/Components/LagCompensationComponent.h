#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"


class AWeapon;
class ABlasterCharacter;
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

	UPROPERTY()
	ABlasterCharacter* Character{};
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

USTRUCT()
struct FServerSideRewindShotgunResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<ABlasterCharacter*, uint32> HeadShots{};
	UPROPERTY()
	TMap<ABlasterCharacter*, uint32> BodyShots{};
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

	
	/*
	 * 	SHOTGUNS
	 */
	UFUNCTION(Server, Reliable)
	void ServerShotgunDamageRequest(
		const TArray<ABlasterCharacter*>& pHitCharacters,
		const FVector_NetQuantize& traceStart,
		const TArray<FVector_NetQuantize>& hitLocations,
		const float hitTime,
		AWeapon* pDamageCauser);

		FServerSideRewindShotgunResult ServerSideRewindShotgun(
		const TArray<ABlasterCharacter*>& pHitCharacter,
		const FVector_NetQuantize& traceStart,
		const TArray<FVector_NetQuantize>& traceEnd,
		const float hitTime);
	
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
	FFramePackage GetFrameToCheck(ABlasterCharacter* pHitCharacter, const float hitTime);


	FServerSideRewindShotgunResult ConfirmHitShotgun(
		const TArray<FFramePackage>& framePackages,
		const FVector_NetQuantize& traceStart,
		const TArray<FVector_NetQuantize>& hitLocations);
};
