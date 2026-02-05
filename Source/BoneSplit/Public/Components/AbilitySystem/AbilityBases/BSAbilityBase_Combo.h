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
	FName MontageStartSection = NAME_None;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0.1))
	float MontageSpeed = 1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MontageRootMotionScale = 1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bIsComboAbility = true;
	                                            
	//If this is set to false it expects a section after the current section to have a 'recover' sequence to idle
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(EditCondition=bIsComboAbility, EditConditionHides))
	bool bAutoBlendFromSectionEnd = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(EditCondition=bIsComboAbility, EditConditionHides))
	FGameplayTag TriggerComboTag = BSTags::AnimEvent_StoreCombo;
	
	UFUNCTION()
	void OnMontageEnded(FGameplayTag Tag, FGameplayEventData EventData);
	
	UFUNCTION()
	void OnEventReceived(FGameplayTag Tag, FGameplayEventData EventData);
	
	virtual void HandleMontageComboEnd(FGameplayTag Tag, FGameplayEventData EventData);
};
