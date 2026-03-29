// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPlayerAbilityBase.h"
#include "BSPlayerAbilityBase_TargetGround.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSPlayerAbilityBase_TargetGround : public UBSPlayerAbilityBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AActor> TargetActorClass;
	
	UPROPERTY(Transient)
	TObjectPtr<AActor> TargetingActor;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AActor> ActorToSpawn;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION()
	void OnAbilityTick(float DeltaSeconds);
	
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnInputReleased(FVector TargetPos, FRotator TargetRot);
};
