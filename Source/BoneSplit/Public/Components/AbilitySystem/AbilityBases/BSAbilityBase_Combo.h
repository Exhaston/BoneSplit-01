// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSAbilityBase.h"
#include "BSAbilityBase_Combo.generated.h"

class UAbilityTask_WaitInputRelease;
class UAbilityTask_WaitInputPress;
class UBSAT_PlayMontageAndWaitForEvent;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSAbilityBase_Combo : public UBSAbilityBase
{
	GENERATED_BODY()
	
public:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* MontageSequence;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag TriggerComboTag = BSTags::AnimEvent_StoreCombo;
	
	UFUNCTION()
	void OnMontageEnded(FGameplayTag Tag, FGameplayEventData EventData);
	
	UFUNCTION()
	void OnEventReceived(FGameplayTag Tag, FGameplayEventData EventData);
};
