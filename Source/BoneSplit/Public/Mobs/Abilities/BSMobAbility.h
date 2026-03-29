// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbilityBase.h"
#include "BSMobAbility.generated.h"

class ABSMobControllerBase;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSMobAbility : public UCharacterAbilityBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, Units=Centimeters))
	float MaxRange = 350;
	
	virtual bool HasValidTarget();
	virtual void ClearTarget();
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	ABSMobControllerBase* GetMobController() const;
	
	UFUNCTION(BlueprintPure)
	AActor* GetCurrentTarget();
	
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> CurrentTarget;
};
