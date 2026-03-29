// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPlayerAbilityBase.h"
#include "BSPlayerAbilityBase_RootMotion.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSPlayerAbilityBase_RootMotion : public UBSPlayerAbilityBase
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintNativeEvent)
	FVector GetMotionDirection();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Strength = 1000;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer GrantedTags;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Duration = 1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUseGravity = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAdditive = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bFinishClampToCharacterMaxSpeed = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float VelocityClampMod = 0.25;
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION()
	void OnMoveCompleted();
	
	
};
