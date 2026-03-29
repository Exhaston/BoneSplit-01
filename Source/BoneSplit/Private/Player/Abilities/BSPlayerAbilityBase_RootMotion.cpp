// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/Abilities/BSPlayerAbilityBase_RootMotion.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"

FVector UBSPlayerAbilityBase_RootMotion::GetMotionDirection_Implementation()
{
	return GetAvatarActorFromActorInfo() ? GetAvatarActorFromActorInfo()->GetActorForwardVector() : FVector::ZeroVector;
}

void UBSPlayerAbilityBase_RootMotion::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		const FVector Dir = GetMotionDirection().GetSafeNormal();
	
		UAbilityTask_ApplyRootMotionConstantForce* RootMotionForce = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
			this, 
			FName("RootMotion"), 
			Dir, 
			Strength, 
			Duration,
			bAdditive, 
			nullptr, 
			ERootMotionFinishVelocityMode::ClampVelocity, 
			GetCharacterAvatar()->GetVelocity(),
			bFinishClampToCharacterMaxSpeed ? 
			GetCharacterAvatar()->GetCharacterMovement()->GetMaxSpeed() * VelocityClampMod : 
			GetCharacterAvatar()->GetVelocity().Length() * VelocityClampMod, 
			bUseGravity);
		
		RootMotionForce->OnFinish.AddDynamic(this, &UBSPlayerAbilityBase_RootMotion::OnMoveCompleted);
		RootMotionForce->ReadyForActivation();
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTags(GrantedTags, 1, EGameplayTagReplicationState::TagAndCountToAll);
		return;
	}
	
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UBSPlayerAbilityBase_RootMotion::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (GetAbilitySystemComponentFromActorInfo())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTags(GrantedTags, 1, EGameplayTagReplicationState::TagAndCountToAll);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBSPlayerAbilityBase_RootMotion::OnMoveCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
