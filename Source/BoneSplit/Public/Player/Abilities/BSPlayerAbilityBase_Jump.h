// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPlayerAbilityBase.h"
#include "BSPlayerAbilityBase_Jump.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, DisplayName="GA_PlayerJump")
class BONESPLIT_API UBSPlayerAbilityBase_Jump : public UBSPlayerAbilityBase
{
	GENERATED_BODY()
	
public:
	
	UBSPlayerAbilityBase_Jump();
	
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayTagContainer* SourceTags = nullptr, 
		const FGameplayTagContainer* TargetTags = nullptr, 
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
};
