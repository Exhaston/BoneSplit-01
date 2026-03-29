// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClientAuthoritativeCharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "BSPlayerMovement.generated.h"


class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSPlayerMovement : public UClientAuthoritativeCharacterMovementComponent
{
	GENERATED_BODY()

public:
	
	UBSPlayerMovement();
	
	virtual void SetupPlayerMovement(UAbilitySystemComponent* InAbilitySystem);
	
	virtual float GetMaxSpeed() const override;
	
	virtual float GetSpeedModifier() const;
	
	virtual bool CanMove() const { return true; }
	
	virtual FRotator ComputeOrientToMovementRotation(
		const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const override;
	
	virtual bool DoJump(bool bReplayingMoves, float DeltaTime) override;
	
	virtual bool CanAttemptJump() const override;
	
	virtual void HandleWalkingOffLedge(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal,
	                           const FVector& PreviousLocation, float TimeDelta) override;
	
	virtual void StartCoyoteTime();
	virtual bool IsCoyoteTimeActive() const;
	virtual void ConsumeCoyoteTime();

protected:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag JumpTag;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(Units="s"))
	float CoyoteTimeDuration = 0.25f;
	
	float CoyoteStartTime = 0.f;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> OwnerAbilitySystem;
};
