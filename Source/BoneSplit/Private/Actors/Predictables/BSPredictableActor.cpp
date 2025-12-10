// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Predictables/BSPredictableActor.h"

#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"


ABSPredictableActor::ABSPredictableActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

bool ABSPredictableActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget,
	const FVector& SrcLocation) const
{
	if (const UGameplayAbility* Ability = GetOwningAbility())
	{
		if (const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
			Info && Info->PlayerController.IsValid())
		{
			if (RealViewer == Info->PlayerController.Get())
			{
				return false;
			}
		}
	}
	
	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

void ABSPredictableActor::InitializePredictableActor(UGameplayAbility* InOwningAbility,
	const FGameplayAbilityTargetDataHandle& TargetData, const bool bShouldReplicate)
{
	OwningAbility = InOwningAbility;
	SetReplicates(bShouldReplicate);
	SetReplicateMovement(false);
}

UGameplayAbility* ABSPredictableActor::GetOwningAbility() const
{
	if (OwningAbility.IsValid()) return OwningAbility.Get();
	return nullptr;
}

