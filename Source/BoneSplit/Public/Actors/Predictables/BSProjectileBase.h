// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "BSProjectileBase.generated.h"

class UBSGameplayEffect;
class UProjectileMovementComponent;
class USphereComponent;

USTRUCT(BlueprintType)
struct FBSProjectileAlignment
{
	GENERATED_BODY()

	FBSProjectileAlignment() = default;
	FBSProjectileAlignment(
		AActor* Owner,
		const FTransform& SpawnTransform,
		const FTransform& CameraTransform);

	bool GetIsValid()   const { return bHasValidData; }
	bool GetIsAligned() const { return bAligned; }
	float GetAlignmentTime(const float DesiredVelocity) const
	{
		const float OffsetDistance = FVector::Dist(StartPos, TargetPos);
		return OffsetDistance / DesiredVelocity;
	}
	
	UPROPERTY()
	bool bAligned = false;
	UPROPERTY()
	bool bHasValidData = false;
	
	UPROPERTY()
	FVector CameraForward = FVector::ForwardVector;
	UPROPERTY()
	FVector StartPos = FVector::ZeroVector;
	UPROPERTY()
	FVector TargetPos = FVector::ZeroVector;
};

UCLASS(NotPlaceable, BlueprintType, Blueprintable, Abstract)
class BONESPLIT_API ABSProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	
	explicit ABSProjectileBase(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;
	
	virtual void SetCollisionProfiles();
	
	static TArray<ABSProjectileBase*> SpawnProjectiles(
		AActor* InOwnerActor, 
		TSubclassOf<ABSProjectileBase> ProjectileClass, 
		const FTransform& InSpawnTransform, 
		int32 NumProjectiles = 0, 
		float SpreadDegrees = 15);
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	UPROPERTY()
	FBSProjectileAlignment ProjectileAlignment;
	
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
protected:
	
	UPROPERTY()
	TArray<AActor*> HitActors;
	
	UPROPERTY()
	float TimeAlive = 0;
	
	UPROPERTY()
	bool bAlignComplete = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bCanPassThroughStatic = false;
	
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
