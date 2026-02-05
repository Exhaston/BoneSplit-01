// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityBases/BSAbilityBase_Combo.h"

#include "AbilitySystemComponent.h"
#include "Components/AbilitySystem/AbilityTasks/BSPlayMontageWaitForEvent.h"

void UBSAbilityBase_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	
	FGameplayTagContainer Tags;
	
	if (bIsComboAbility)
	{
		Tags.AddTagFast(TriggerComboTag);
	}
	
	UBSAT_PlayMontageAndWaitForEvent* MontageTask = UBSAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
		this, 
		"ComboMontageTask",
		MontageSequence,
		Tags,
		MontageSpeed,
		MontageStartSection,
		bAutoBlendFromSectionEnd, //Avoid this montage end section to end when the ability actually ended.
		MontageRootMotionScale);
	
	MontageTask->OnInterrupted.AddDynamic(this, &UBSAbilityBase_Combo::OnMontageEnded);
	MontageTask->OnCompleted.AddDynamic(this, &UBSAbilityBase_Combo::OnMontageEnded);
	MontageTask->EventReceived.AddDynamic(this, &UBSAbilityBase_Combo::OnEventReceived);
	MontageTask->ReadyForActivation();
}

void UBSAbilityBase_Combo::OnMontageEnded(FGameplayTag Tag, FGameplayEventData EventData)
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), 
		true, false);
}

void UBSAbilityBase_Combo::OnEventReceived(const FGameplayTag Tag, FGameplayEventData EventData)
{
	if (bIsComboAbility)
	{
		HandleMontageComboEnd(Tag, EventData);
	}
}

void UBSAbilityBase_Combo::HandleMontageComboEnd(FGameplayTag Tag, FGameplayEventData EventData)
{
	if (Tag.MatchesTagExact(TriggerComboTag))
	{
		if (!GetCurrentAbilitySpec()->InputPressed)
		{
			if (const UAnimInstance* AnimInstance = GetCurrentActorInfo()->GetAnimInstance(); 
				AnimInstance && bAutoBlendFromSectionEnd)
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
