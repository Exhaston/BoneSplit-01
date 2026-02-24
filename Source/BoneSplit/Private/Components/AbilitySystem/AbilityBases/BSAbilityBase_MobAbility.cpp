// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityBases/BSAbilityBase_MobAbility.h"

#include "AIController.h"
#include "Actors/Mob/BSMobCharacter.h"
#include "Actors/Mob/BSMobSpawner.h"
#include "Actors/Mob/BSTargetSetting.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/AbilitySystem/AbilityTasks/BSPlayMontageWaitForEvent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBSAbilityBase_MobAbility::UBSAbilityBase_MobAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

bool UBSAbilityBase_MobAbility::CommitCheck(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            FGameplayTagContainer* OptionalRelevantTags)
{
	if (!ActorInfo->AvatarActor.IsValid()) return false;

	switch (TargetMode)
	{
	case EBSTargetMode::EBSTM_HighestThreat:
		CurrentTarget = GetHighestThreatTarget();
		break;
	case EBSTargetMode::EBSTM_LowestThreat:
		CurrentTarget = GetLowestThreatTarget();
		break;
	case EBSTargetMode::EBSTM_Random:
		CurrentTarget = GetRandomThreatTarget();
		break;
	default: 
		CurrentTarget = GetHighestThreatTarget();
		break;
	}
	
	if (!CurrentTarget.IsValid()) return false;

	return Super::CommitCheck(Handle,ActorInfo,ActivationInfo,OptionalRelevantTags);
}

bool UBSAbilityBase_MobAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bInRange = 
	FVector::Dist(CurrentTarget->GetActorLocation(), ActorInfo->AvatarActor->GetActorLocation()) <= AbilityRange;
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) && bInRange;
}

void UBSAbilityBase_MobAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UBSAbilityBase_MobAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const bool bReplicateEndAbility, const bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AActor* UBSAbilityBase_MobAbility::GetRandomThreatTarget()
{
	return GetOwnerCharacter()->GetThreatComponent()->GetRandomThreatActor();
}

AActor* UBSAbilityBase_MobAbility::GetHighestThreatTarget()
{
	return GetOwnerCharacter()->GetThreatComponent()->GetHighestThreatActor();
}

AActor* UBSAbilityBase_MobAbility::GetLowestThreatTarget()
{
	return GetOwnerCharacter()->GetThreatComponent()->GetLowestThreatActor();
}

AActor* UBSAbilityBase_MobAbility::GetCurrentTargetActor()
{
	return CurrentTarget.IsValid() ? CurrentTarget.Get() : nullptr;
}

ABSMobCharacter* UBSAbilityBase_MobAbility::GetOwnerCharacter()
{
	return Cast<ABSMobCharacter>(GetAvatarActorFromActorInfo());
}
