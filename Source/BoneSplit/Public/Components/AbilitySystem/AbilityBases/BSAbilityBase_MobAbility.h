// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSAbilityBase.h"
#include "BSAbilityBase_MobAbility.generated.h"


class AAIController;

namespace EPathFollowingResult
{
	enum Type : int;
}

class UBSTargetSetting;
class UBSTargetMode;

UCLASS()
class BONESPLIT_API UBSAbilityBase_MobAbility : public UBSAbilityBase
{
	GENERATED_BODY()
	
public:
	
	UBSAbilityBase_MobAbility();
	
	virtual bool CommitCheck(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		FGameplayTagContainer* OptionalRelevantTags = nullptr) override;
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentTarget;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bAutoMoveToTarget = true;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bRequireGrounded = true;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bRequireToken = true;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bAutoReleaseToken = true;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float StoppingDistance = 100;
	
	//For storing tasks to avoid garbage collection
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced)
	UBSTargetSetting* TargetSetting;
	
	UFUNCTION(BlueprintPure)
	AActor* GetTarget() const;
	
	UFUNCTION(BlueprintCallable, meta=(AdvancedDisplay="Rate, RootMotionScale"))
	void StartMontageTask(UAnimMontage* MontageAsset, 
		FGameplayTagContainer EventTags, float Rate, float RootMotionScale);
	
	UFUNCTION()
	void OnMontageFinished(FGameplayTag Tag, FGameplayEventData Payload);
	
	UFUNCTION()
	void OnMontageEventReceived(FGameplayTag Tag, FGameplayEventData Payload);
	
	virtual void MontageEventReceived(FGameplayTag, FGameplayEventData Payload);
};
