// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPlayerAbilityBase.h"
#include "BSPlayerAbilityBase_HookShot.generated.h"

class ABSHookPoint;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSPlayerAbilityBase_HookShot : public UBSPlayerAbilityBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Hook")
	float MoveSpeed = 3500;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hook")
	float HookRange = 1000;
	
	virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION()
	void OnHookTick(float DeltaSeconds);
	
	UFUNCTION()
	void OnMoveToHookCompleted();

	UPROPERTY(Transient)
	TWeakObjectPtr<ABSHookPoint> CachedHookPoint;
};
