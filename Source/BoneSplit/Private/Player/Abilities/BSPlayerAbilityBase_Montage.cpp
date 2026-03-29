// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/Abilities/BSPlayerAbilityBase_Montage.h"

#include "Abilities/BSExtendedAttributeSet.h"
#include "Tasks/AbilityTask_PlayMontageWaitForEvent.h"

namespace BSGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(AnimEvent_StoreCombo, "AnimEvent.StoreCombo");
}

void UBSPlayerAbilityBase_Montage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	
	FGameplayTagContainer& Tags = EventTagsToListenTo;
	
	if (bIsComboAbility)
	{
		Tags.AddTagFast(StoreComboTag);
	}
	
	UAbilityTask_PlayMontageWaitForEvent* MontageTask = UAbilityTask_PlayMontageWaitForEvent::PlayMontageAndWaitForEvent(
		this, 
		"ComboMontageTask",
		MontageSequence,
		Tags,
		UBSExtendedAttributeSet::GetAttackSpeedAttribute(),
		MontageSpeed,
		MontageStartSection,
		false, //Avoid this montage end section to end when the ability actually ended.
		MontageRootMotionScale);
	
	if (bEndOnBlendOut) MontageTask->OnBlendOut.AddDynamic(this, &UBSPlayerAbilityBase_Montage::OnMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &UBSPlayerAbilityBase_Montage::OnMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &UBSPlayerAbilityBase_Montage::OnMontageEnded);
	MontageTask->OnCompleted.AddDynamic(this, &UBSPlayerAbilityBase_Montage::OnMontageEnded);
	MontageTask->EventReceived.AddDynamic(this, &UBSPlayerAbilityBase_Montage::OnEventReceived);
	MontageTask->ReadyForActivation();
}

void UBSPlayerAbilityBase_Montage::OnMontageEnded(FGameplayTag Tag, FGameplayEventData EventData)
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), 
		true, false);
}

void UBSPlayerAbilityBase_Montage::OnEventReceived(const FGameplayTag Tag, FGameplayEventData EventData)
{
	BP_OnGameplayEventReceived(Tag, EventData);
	if (bIsComboAbility)
	{
		HandleMontageComboEnd(Tag, EventData);
	}
}

void UBSPlayerAbilityBase_Montage::BP_OnGameplayEventReceived_Implementation(FGameplayTag EventTag,
	const FGameplayEventData Payload)
{
	
}

void UBSPlayerAbilityBase_Montage::HandleMontageComboEnd(const FGameplayTag Tag, FGameplayEventData EventData)
{
	if (Tag.MatchesTagExact(StoreComboTag))
	{
		if (!GetCurrentAbilitySpec()->InputPressed)
		{
			if (const UAnimInstance* AnimInstance = GetCurrentActorInfo()->GetAnimInstance(); AnimInstance)
			{
				const FName CurrentSection = AnimInstance->Montage_GetCurrentSection(GetCurrentMontage());
				
				const FName NextSection = 
					MontageSequence->GetSectionName(MontageSequence->GetSectionIndex(CurrentSection) + 1);
				
				MontageSetNextSectionName(CurrentSection, NextSection);
			}
			
			EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), 
			true, false);
		}
	}
}
