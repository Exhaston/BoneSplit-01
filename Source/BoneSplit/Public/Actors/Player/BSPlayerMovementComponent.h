// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClientAuthoritativeCharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "BSPlayerMovementComponent.generated.h"


class IBSMovementInterface;
class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSPlayerMovementComponent : public UClientAuthoritativeCharacterMovementComponent
{
	GENERATED_BODY()

public:
	
	UBSPlayerMovementComponent();
	
	virtual void BeginPlay() override;
	
	virtual void BindTags(UAbilitySystemComponent* InAsc);
	
	virtual void BindAttributes(UAbilitySystemComponent* InAsc);
	
	// =================================================================================================================
	// Jumping
	// ================================================================================================================= 
	
	virtual bool CanAttemptJump() const override;
	
	virtual bool DoJump(bool bReplayingMoves, float DeltaTime) override;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag JumpUnlockTag = BSTags::Talent_Jump;
	
	UPROPERTY()
	int32 JumpTags = 0;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayAttribute JumpHeightAttribute = UBSAttributeSet::GetJumpHeightAttribute();
	
	//Cached from asc, bound to 
	UPROPERTY()
	float CachedJumpHeight = 150;
	
	// =================================================================================================================
	// Speed & Acceleration (Fake friction)
	// =================================================================================================================
	
	virtual bool CanMove() const;
	
	virtual float GetMaxAcceleration() const override;
	
	virtual float GetMaxBrakingDeceleration() const override;
	
	virtual float GetMaxSpeed() const override;
	
	UPROPERTY()
	TArray<FGameplayTag> ImpairmentTags;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayAttribute SpeedAttribute = UBSAttributeSet::GetSpeedAttribute();
	
	//Multiplier to the movement speed.
	UPROPERTY()
	float CachedSpeedMod = 1;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayAttribute FrictionAttribute = UBSAttributeSet::GetFrictionAttribute();
	
	UPROPERTY()
	float CachedAccelerationMod = 1;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayAttribute AirControlAttribute = UBSAttributeSet::GetAirControlAttribute();
	
	// =================================================================================================================
	// Coyote Time
	// =================================================================================================================
	
	virtual void HandleWalkingOffLedge(
		const FVector& PreviousFloorImpactNormal, 
		const FVector& PreviousFloorContactNormal, 
		const FVector& PreviousLocation, 
		float TimeDelta) override;
	
	virtual void StartCoyoteTime();
	
	//Returns true if coyote time is active. This will be active after leaving an edge toward CoyoteTimeDuration
	UFUNCTION(BlueprintPure)
	bool IsCoyoteTimeActive() const;
	
	//Resets and invalidates coyote time and refunds current jump increment (this still counts as on ground)
	virtual void ConsumeCoyoteTime();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(Units="s"))
	float CoyoteTimeDuration = 0.25;
	
	UPROPERTY()
	float CoyoteStartTime = 0;
	
	// =================================================================================================================
	// Asc
	// =================================================================================================================
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> OwnerAsc;
	
	UPROPERTY()
	TScriptInterface<IBSMovementInterface> MovementInterface;
};
