// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectileBullet::AProjectileBullet()
{
	//Set up movement component
	m_pProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	m_pProjectileMovementComponent->bRotationFollowsVelocity = true;
	m_pProjectileMovementComponent->SetIsReplicated(true);
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	FPredictProjectilePathParams pathParams{};
	pathParams.bTraceWithChannel = true;
	pathParams.bTraceWithCollision = true;
	pathParams.DrawDebugTime = 5.f;
	pathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	pathParams.LaunchVelocity = GetActorForwardVector() * m_pProjectileMovementComponent->InitialSpeed;
	pathParams.MaxSimTime = 4.f;
	pathParams.ProjectileRadius = 5.f;
	pathParams.SimFrequency = 30.f;
	pathParams.StartLocation = GetActorLocation();
	pathParams.TraceChannel = ECC_Visibility;
	pathParams.ActorsToIgnore.Add(this);
	FPredictProjectilePathResult pathResult{};
	
	UGameplayStatics::PredictProjectilePath(this, pathParams, pathResult);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	const ACharacter* ownerCharacter = Cast<ACharacter>(GetOwner());
	if (!ownerCharacter) return;

	AController* ownerController = ownerCharacter->GetController();
	if (!ownerController) return;

	if (!OtherActor) return;
	
	UGameplayStatics::ApplyDamage(OtherActor, m_Damage, ownerController, this, UDamageType::StaticClass());
	
	//Super should be called last since we are calling the Destroy function in it
	Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
}
