// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPlayerAbilityBase.h"
#include "NativeGameplayTags.h"
#include "UObject/Object.h"
#include "BSPlayerAbilityBase_Montage.generated.h"

namespace BSGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimEvent_StoreCombo);
}

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSPlayerAbilityBase_Montage : public UBSPlayerAbilityBase
{
	GENERATED_BODY()
	
public:
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="AnimEvent"))
	FGameplayTagContainer EventTagsToListenTo;
	
	//Montage to play. Can be any slot montage.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* MontageSequence;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bEndOnBlendOut = false;
	
	//Montage Section to start from.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName MontageStartSection = NAME_None;
	
	//Montage speed (rate) multiplier
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0.1))
	float MontageSpeed = 1;
	
	//Root motion scale of the montage, can have unexpected results with very high values.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MontageRootMotionScale = 1;
	
	//Set to true if this ability is to be held for a looping animation with breakpoints in notifies
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bIsComboAbility = true;
	                                                                                                       
	//Default to AnimEvent.StoreCombo, this is the event we listen to before an attack section ends. 
	//If input is held at this event we continue the Montage. if not, it plays the next section (set up to be return anim)
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(EditCondition=bIsComboAbility, EditConditionHides))
	FGameplayTag StoreComboTag = BSGameplayTags::AnimEvent_StoreCombo;
	
	UFUNCTION()
	void OnMontageEnded(FGameplayTag Tag, FGameplayEventData EventData);
	
	UFUNCTION()
	void OnEventReceived(FGameplayTag Tag, FGameplayEventData EventData);
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnGameplayEvent")
	void BP_OnGameplayEventReceived(FGameplayTag EventTag, const FGameplayEventData Payload);
	
	virtual void HandleMontageComboEnd(FGameplayTag Tag, FGameplayEventData EventData);
};
