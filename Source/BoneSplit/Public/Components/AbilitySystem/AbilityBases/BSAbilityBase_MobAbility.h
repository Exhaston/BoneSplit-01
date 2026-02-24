// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSAbilityBase.h"
#include "BSAbilityBase_MobAbility.generated.h"


class ABSMobCharacter;
class AAIController;

namespace EPathFollowingResult
{
	enum Type : int;
}

class UBSTargetSetting;
class UBSTargetMode;

UENUM(BlueprintType)
enum EBSTargetMode : uint8
{
	EBSTM_HighestThreat  = 0 UMETA(DisplayName="HighestThreat"),
	EBSTM_LowestThreat   = 1 UMETA(DisplayName="LowestThreat"),
	EBSTM_Random         = 2 UMETA(DisplayName="RandomThreat"),
};                         

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
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION(BlueprintCallable)
	AActor* GetRandomThreatTarget();
	
	UFUNCTION(BlueprintCallable)
	AActor* GetHighestThreatTarget();
	
	UFUNCTION(BlueprintCallable)
	AActor* GetLowestThreatTarget();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float AbilityRange = 100;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TEnumAsByte<EBSTargetMode> TargetMode;
	
	UFUNCTION(BlueprintPure)
	AActor* GetCurrentTargetActor();
	
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentTarget;
	
	UFUNCTION(BlueprintPure)
	ABSMobCharacter* GetOwnerCharacter();
};
