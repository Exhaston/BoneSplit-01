// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityBases/BSAbilityBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayTask.h"
#include "Actors/Predictables/BSPredictableActor.h"

UBSAbilityBase::UBSAbilityBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	//*Most* abilities will block other abilities
	BlockAbilitiesWithTag = FGameplayTagContainer(BSTags::Ability);
}

void UBSAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		EventHandle = ActorInfo->AbilitySystemComponent.Get()->AddGameplayEventTagContainerDelegate(EventTagsToListenTo, 
		FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
		this, &UBSAbilityBase::Native_OnGameplayEventReceived));
	}
}

void UBSAbilityBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

TSoftObjectPtr<UTexture2D> UBSAbilityBase::GetIcon_Implementation() const
{
	return AbilityIcon;
}

void UBSAbilityBase::Native_OnGameplayEventReceived(const FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	BP_OnGameplayEventReceived(EventTag, *Payload);
}

void UBSAbilityBase::BP_OnGameplayEventReceived_Implementation(FGameplayTag EventTag, const FGameplayEventData Payload)
{
}

void UBSAbilityBase::SpawnPredictedActor(
	const TSubclassOf<ABSPredictableActor> ActorToSpawn,
	const FTransform& SpawnTransform, const FGameplayAbilityTargetDataHandle& TargetData)
{
	//Spawn instantly for the caller
	ABSPredictableActor* PredictedActor =
		GetAvatarActorFromActorInfo()->GetWorld()->SpawnActorDeferred<ABSPredictableActor>(
			ActorToSpawn,
			SpawnTransform,
			GetAvatarActorFromActorInfo(),
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	PredictedActor->InitializePredictableActor(this, TargetData, 
		GetAvatarActorFromActorInfo()->HasAuthority());
	
	PredictedActor->FinishSpawning(SpawnTransform);
}

void UBSAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const bool bReplicateEndAbility, const bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (EventHandle.IsValid() && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent.Get()->RemoveGameplayEventTagContainerDelegate(
			EventTagsToListenTo, EventHandle);
		
		EventHandle.Reset();
	}
}
