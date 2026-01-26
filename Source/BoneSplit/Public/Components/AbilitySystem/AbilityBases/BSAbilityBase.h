// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IconThumbnailInterface.h"
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
class BONESPLIT_API UBSAbilityBase : public UGameplayAbility, public IIconThumbnailInterface
{
	GENERATED_BODY()
	
public:
	
	UBSAbilityBase();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation() const override;
	
	void Native_OnGameplayEventReceived(FGameplayTag EventTag, const FGameplayEventData* Payload);
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnGameplayEvent")
	void BP_OnGameplayEventReceived(FGameplayTag EventTag, const FGameplayEventData Payload);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UTexture2D> AbilityIcon;
	
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
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="AnimEvent"))
	FGameplayTagContainer EventTagsToListenTo;
	
	FDelegateHandle EventHandle;
};
