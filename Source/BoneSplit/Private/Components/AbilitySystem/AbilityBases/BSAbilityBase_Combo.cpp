// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityBases/BSAbilityBase_Combo.h"

#include "AbilitySystemComponent.h"
#include "Components/AbilitySystem/AbilityTasks/BSPlayMontageWaitForEvent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

bool UBSAbilityBase_Combo::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                              const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (bRequireGrounded && ActorInfo->AvatarActor.IsValid() && Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (Cast<ACharacter>(ActorInfo->AvatarActor.Get())->GetCharacterMovement()->IsFalling()) return false;
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

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
		bScaleWithAttackSpeed,
		MontageStartSection,
		false, //Avoid this montage end section to end when the ability actually ended.
		MontageRootMotionScale);
	
	MontageTask->OnCancelled.AddDynamic(this, &UBSAbilityBase_Combo::OnMontageEnded);
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
				AnimInstance && !bAutoBlendFromSectionEnd)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("DID IT"));
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
