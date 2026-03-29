// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayAbilities/Public/AbilitySystemComponent.h"
#include "BSProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UAbilitySystemComponent;

UCLASS(NotPlaceable, BlueprintType, Blueprintable, Abstract)
class BONESPLIT_API ABSProjectile : public AActor
{
	GENERATED_BODY()

public:
	
	explicit ABSProjectile(const FObjectInitializer& ObjectInitializer);
	
	virtual void PostInitializeComponents() override;
	
	virtual void SetCollisionProfiles();
	
	static TArray<ABSProjectile*> SpawnProjectiles(
		AActor* InOwnerActor, 
		TSubclassOf<ABSProjectile> ProjectileClass, 
		const FTransform& InSpawnTransform, 
		int32 NumProjectiles = 0, 
		float SpreadDegrees = 15);
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
protected:
	
	UFUNCTION(BlueprintPure)
	UAbilitySystemComponent* GetAbilitySystemFromOwner() const { return GetAbilitySystemComponent(); }
	
	UFUNCTION(BlueprintNativeEvent)
	bool OnAnyTargetHit(UAbilitySystemComponent* TargetAsc);
	
	UPROPERTY()
	TArray<AActor*> HitActors;
	
	UPROPERTY()
	float TimeAlive = 0;
	
	UPROPERTY()
	bool bAlignComplete = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bCanPassThroughStatic = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(ClampMin=1))
	int32 MaxHits = 1;
	
	UPROPERTY()
	int32 CurrentHits = 0;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USphereComponent* OverlapComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UProjectileMovementComponent* ProjectileMovementComponent;
};
