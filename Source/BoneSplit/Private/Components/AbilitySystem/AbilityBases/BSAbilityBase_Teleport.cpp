// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityBases/BSAbilityBase_Teleport.h"

#include "AbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/AbilitySystem/BSAbilityLibrary.h"
#include "Components/AbilitySystem/AbilityTasks/BSPlayMontageWaitForEvent.h"

void UBSAbilityBase_Teleport::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	
	if (!CurrentTargetActor.IsValid())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
	
	COMMIT_ABILITY(Handle, ActorInfo, ActivationInfo)
	
	if (IntroMontage)
	{
		UAbilityTask_PlayMontageAndWait* Montage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, 
		"IntroMontage",
		IntroMontage, 
		1);
		
		Montage->OnCompleted.AddDynamic(this, &UBSAbilityBase_Teleport::OnIntroMontageCompleted);
		Montage->ReadyForActivation();
	}
	else
	{
		OnIntroMontageCompleted();
	}
}

void UBSAbilityBase_Teleport::OnIntroMontageCompleted()
{
	if (!CurrentTargetActor.IsValid()) return;
	AActor* Avatar = GetAvatarActorFromActorInfo();
	const AActor* Target = CurrentTargetActor.Get();
	if (!Avatar->TeleportTo(Target->GetTransform().TransformPosition(TargetOffset), 
	Target->GetActorRotation()))
	{
		CancelAbility(
			GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}
}
