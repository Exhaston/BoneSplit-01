// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSAbilityBase.h"
#include "BSAbilityBase_PlayerHook.generated.h"

class UCurveVector;
class ABSHookPoint;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSAbilityBase_PlayerHook : public UBSAbilityBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Hook")
	float MoveSpeed = 3500;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float HookRange = 1000;
	
	virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION()
	void OnHookTick(float DeltaSeconds);
	
	UFUNCTION()
	void OnMoveToHookCompleted();

	UPROPERTY()
	TWeakObjectPtr<ABSHookPoint> CachedHookPoint;
	
};


