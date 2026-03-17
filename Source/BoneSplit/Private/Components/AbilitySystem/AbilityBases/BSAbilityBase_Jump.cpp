// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityBases/BSAbilityBase_Jump.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBSAbilityBase_Jump::UBSAbilityBase_Jump()
{
	InputID = EAII_Jump;
	BufferChannel = EABT_Independent;
	ActivationGroup = EAG_Independent;
}

bool UBSAbilityBase_Jump::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, 
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;
	return GetCharacterAvatar()->CanJump();
}

void UBSAbilityBase_Jump::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		GetCharacterAvatar()->Jump();
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}