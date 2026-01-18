// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BoneSplit/BoneSplit.h"
#include "BSAbilityBase.generated.h"

enum EBSAbilityInputID : uint8;
class ABSPredictableActor;

#define COMMIT_ABILITY(Handle, ActorInfo, ActivationInfo)   \
if (!CommitAbility(Handle, ActorInfo, ActivationInfo))      \
{                                                           \
	CancelAbility(Handle, ActorInfo, ActivationInfo, true); \
	return;                                                 \
}
#define CANCEL_ABILITY() \
CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true); return;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DisplayName="Ability Base")
class BONESPLIT_API UBSAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	UBSAbilityBase();
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Input")
	TEnumAsByte<EBSAbilityInputID> PlayerInputID = None;
	
	virtual void SpawnPredictedActor(const TSubclassOf<ABSPredictableActor> ActorToSpawn,
	const FTransform& SpawnTransform,
	const FGameplayAbilityTargetDataHandle& TargetData);
	
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		bool bReplicateEndAbility, 
		bool bWasCancelled) override;
	
protected:
	
	UPROPERTY()
	TArray<UGameplayTask*> Tasks;
};
