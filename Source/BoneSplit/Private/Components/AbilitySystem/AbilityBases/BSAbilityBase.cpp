// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityBases/BSAbilityBase.h"

#include "Actors/Predictables/BSPredictableActor.h"

UBSAbilityBase::UBSAbilityBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UBSAbilityBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	
	if (FGameplayAbilitySpec* CurrentSpec = GetCurrentAbilitySpec(); PlayerInputID != None)
	{
		CurrentSpec->InputID = PlayerInputID;
	}
}

void UBSAbilityBase::SpawnPredictedActor(const TSubclassOf<ABSPredictableActor> ActorToSpawn,
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
