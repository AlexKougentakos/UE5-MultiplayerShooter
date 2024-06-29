// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	const ACharacter* ownerCharacter = Cast<ACharacter>(GetOwner());
	if (!ownerCharacter) return;

	AController* ownerController = ownerCharacter->GetController();
	if (!ownerController) return;
	
	UGameplayStatics::ApplyDamage(OtherActor, m_Damage, ownerController, this, UDamageType::StaticClass());
	
	//Super should be called last since we are calling the Destroy function in it
	Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
}
