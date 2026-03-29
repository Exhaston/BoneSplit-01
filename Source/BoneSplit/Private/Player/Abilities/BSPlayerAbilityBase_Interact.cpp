// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/Abilities/BSPlayerAbilityBase_Interact.h"

#include "Player/Interactions/BSPlayerInteractionComponent.h"

bool UBSPlayerAbilityBase_Interact::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                                       const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (const UBSPlayerInteractionComponent* PlayerInteractionComponent = 
		UBSPlayerInteractionComponent::GetInteractionComponent(GetAvatarActorFromActorInfo()))
	{
		if (!PlayerInteractionComponent->GetFocusedActor()) return false;
	}
	
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBSPlayerAbilityBase_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (const UBSPlayerInteractionComponent* PlayerInteractionComponent = 
		UBSPlayerInteractionComponent::GetInteractionComponent(GetAvatarActorFromActorInfo()))
		{
			PlayerInteractionComponent->TryInteract();
		}
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}
