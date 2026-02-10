// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "BSProjectileBase.generated.h"

class UBSGameplayEffect;
class UProjectileMovementComponent;
class USphereComponent;

USTRUCT()
struct FBSProjectileAlignment
{
	GENERATED_BODY()
	
	FBSProjectileAlignment() = default;
	
	FBSProjectileAlignment(
		const FTransform& SpawnTransform,
		const FVector& StartVelocity, 
		const FTransform& CameraTransform, 
		float InAlignmentTime);
	
	FVector EvaluatePosition(float TimeAlive, const FVector& GravityAcceleration);
	
	//Returns true if this projectile was set up correctly with adjustments needed for a player
	bool GetIsValid() const { return bHasValidData; }
	
	bool GetIsAligned() const { return bAligned; }
	
	static FVector Slerp(const FVector& a, const FVector& b, const float t)
	{
		float omega = FGenericPlatformMath::Acos(FVector::DotProduct(
				a.GetSafeNormal(), 
				b.GetSafeNormal()
				));
		float sinOmega = FGenericPlatformMath::Sin(omega);
		FVector termOne = a * (FGenericPlatformMath::Sin(omega * (1.0 - t)) / sinOmega);
		FVector termTwo = b * (FGenericPlatformMath::Sin(omega * (      t)) / sinOmega);
		return termOne + termTwo;
	}

protected:
	
	UPROPERTY()
	FVector StartPos;
	UPROPERTY()
	FVector StartVel;
	UPROPERTY()
	FVector RealStartPos;
	UPROPERTY()
	FVector RealStartVel;
	UPROPERTY()
	bool bAligned = false;
	UPROPERTY()
	bool bHasValidData = false;
	UPROPERTY()
	float AlignmentTime = 0;
};

UCLASS(NotPlaceable, BlueprintType, Blueprintable, Abstract)
class BONESPLIT_API ABSProjectileBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	
	explicit ABSProjectileBase(const FObjectInitializer& ObjectInitializer);
	
	static ABSProjectileBase* SpawnProjectile(AActor* InOwnerActor, TSubclassOf<ABSProjectileBase> ProjectileClass, const FTransform& InSpawnTransform, const FTransform& InCameraTransform);
	
	static ABSProjectileBase* SpawnProjectile(AActor* InOwnerActor, TSubclassOf<ABSProjectileBase> ProjectileClass, const FTransform& InSpawnTransform);
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UPROPERTY()
	FBSProjectileAlignment ProjectileAlignment;
	
protected:
	
	UPROPERTY()
	TArray<AActor*> HitActors;
	
	UPROPERTY()
	float TimeAlive = 0;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bHitFriendlies = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bHitEnemies = true;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UBSGameplayEffect> EffectClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(ClampMin=1))
	int32 MaxHits = 1;
	
	UPROPERTY()
	int32 CurrentHits = 0;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USphereComponent* OverlapComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UProjectileMovementComponent* ProjectileMovementComponent;
};
