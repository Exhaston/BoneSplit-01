// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TickTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);

/**
 * 
 */
UCLASS(DisplayName="Ability Tick Task")
class CHARACTERABILITIES_API UAbilityTask_TickTask : public UAbilityTask
{
	GENERATED_BODY()

public:                            
	
	explicit UAbilityTask_TickTask(const FObjectInitializer& ObjectInitializer);
 
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", 
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_TickTask* AbilityTaskOnTick(UGameplayAbility* OwningAbility, FName TaskInstanceName);
	
	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime) override;
	
	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;
};
