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
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	//Montage to play. Can be any slot montage.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* MontageSequence;
	
	//Montage Section to start from.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName MontageStartSection = NAME_None;
	
	//Montage speed (rate) multiplier
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0.1))
	float MontageSpeed = 1;
	
	//Set to true if montage speed should be multiplied with the attack speed attribute.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bScaleWithAttackSpeed = true;
	
	//Root motion scale of the montage, can have unexpected results with very high values.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MontageRootMotionScale = 1;
	
	//Checks if the avatar actor is grounded in CanActivate()
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bRequireGrounded = true;
	
	//Set to true if this ability is to be held for a looping animation with breakpoints in notifies
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bIsComboAbility = true;
	                                            
	//If false, assumes Sections is set up so next section of an attack is return to idle / default pose
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(EditCondition=bIsComboAbility, EditConditionHides))
	bool bAutoBlendFromSectionEnd = false;
	                                                                                                       
	//Default to AnimEvent.StoreCombo, this is the event we listen to before an attack section ends. 
	//If input is held at this event we continue the Montage. if not, it plays the next section (set up to be return anim)
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(EditCondition=bIsComboAbility, EditConditionHides))
	FGameplayTag TriggerComboTag = BSTags::AnimEvent_StoreCombo;
	
	UFUNCTION()
	void OnMontageEnded(FGameplayTag Tag, FGameplayEventData EventData);
	
	UFUNCTION()
	void OnEventReceived(FGameplayTag Tag, FGameplayEventData EventData);
	
	virtual void HandleMontageComboEnd(FGameplayTag Tag, FGameplayEventData EventData);
};
