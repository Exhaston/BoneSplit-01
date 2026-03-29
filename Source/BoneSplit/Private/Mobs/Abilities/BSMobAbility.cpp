// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Mobs/Abilities/BSMobAbility.h"

#include "GameFramework/Character.h"
#include "Mobs/BSMobControllerBase.h"

bool UBSMobAbility::HasValidTarget()
{
	if (GetMobController() && GetMobController()->GetHighestThreatActor())
	{
		
		AActor* TargetActor = GetMobController()->GetHighestThreatActor();
		if (TargetActor && FVector::Distance(TargetActor->GetActorLocation(), GetAvatarActorFromActorInfo()->GetActorLocation()) <= MaxRange && GetMobController()->LineOfSightTo(TargetActor, GetAvatarActorFromActorInfo()->GetTargetLocation(), false))
		{
			CurrentTarget = TargetActor;
		}
	}
	
	return CurrentTarget.IsValid();
}

void UBSMobAbility::ClearTarget()
{
	CurrentTarget = nullptr;
}

bool UBSMobAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                       const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) && GetMobController() && CurrentTarget.IsValid();
}

void UBSMobAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CurrentTarget.IsValid() || !GetMobController())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	
	GetMobController()->SetCurrentTarget(CurrentTarget.Get());
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	

}

void UBSMobAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CurrentTarget = nullptr;
	if (GetMobController())
	{
		GetMobController()->ClearCurrentTarget();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

ABSMobControllerBase* UBSMobAbility::GetMobController() const
{
	return GetCharacterAvatar()->GetController<ABSMobControllerBase>();
}

AActor* UBSMobAbility::GetCurrentTarget()
{
	return CurrentTarget.IsValid() ? CurrentTarget.Get() : nullptr;
}
