// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSMobProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class BONESPLIT_API ABSMobProjectile : public AActor
{
	GENERATED_BODY()

public:
	
	explicit ABSMobProjectile(const FObjectInitializer& ObjectInitializer);
	void SetCollisionProfiles();
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ExposeOnSpawn))
	AActor* TargetActor;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USphereComponent* OverlapComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UProjectileMovementComponent* ProjectileMovementComponent;
};
