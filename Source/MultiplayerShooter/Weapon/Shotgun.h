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
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Fire(const FVector& hitTarget) override;
private:

	UPROPERTY(EditAnywhere, DisplayName = "Number of Pellets", Category = "Weapon Stats")
	int m_NumberOfPellets{ 10 };
};
