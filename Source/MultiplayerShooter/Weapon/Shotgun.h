#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	AShotgun();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void FireShotgun(const TArray<FVector_NetQuantize>& hitLocations);
	void ShotgunGetVectorWithSpread(const FVector& hitTarget, TArray<FVector_NetQuantize>& outShotLocations) const;
private:

	UPROPERTY(EditAnywhere, DisplayName = "Number of Pellets", Category = "Weapon Stats|Shooting")
	int m_NumberOfPellets{ 10 };
};