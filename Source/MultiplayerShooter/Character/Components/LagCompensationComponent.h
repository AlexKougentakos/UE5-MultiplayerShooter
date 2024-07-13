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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYERSHOOTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULagCompensationComponent();
	friend class ABlasterCharacter;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ShowFramePackage(const FFramePackage& framePackage);
private:
	ABlasterCharacter* m_pCharacter{};
	ABlasterPlayerController* m_pBlasterPlayerController{};

	void SaveFramePackage(FFramePackage& framePackage);
};
