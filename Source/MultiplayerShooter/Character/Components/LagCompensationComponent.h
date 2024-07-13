#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/RingBuffer.h"
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

	void ShowFramePackage(const FFramePackage& framePackage);
private:
	ABlasterCharacter* m_pCharacter{};
	ABlasterPlayerController* m_pBlasterPlayerController{};

	UPROPERTY(EditAnywhere, Category = "Lag Compensation", DisplayName = "Max Recording Time")
	float m_MaxRecordingTime{4.f};
	
	TRingBuffer<FFramePackage> m_FrameHistory{};
	void SaveFramePackage(FFramePackage& framePackage);
};
