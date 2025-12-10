// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BSAbilityBase.generated.h"

class ABSPredictableActor;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DisplayName="Ability Base")
class BONESPLIT_API UBSAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	UBSAbilityBase();           
	
	virtual void SpawnPredictedActor(const TSubclassOf<ABSPredictableActor> ActorToSpawn,
	const FTransform& SpawnTransform,
	const FGameplayAbilityTargetDataHandle& TargetData);
};
