// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Tasks/AbilityTask_TickTask.h"

UAbilityTask_TickTask::UAbilityTask_TickTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bTickingTask = true;
}
 
UAbilityTask_TickTask* UAbilityTask_TickTask::AbilityTaskOnTick(UGameplayAbility* OwningAbility, const FName TaskInstanceName)
{
	UAbilityTask_TickTask* MyObj = NewAbilityTask<UAbilityTask_TickTask>(OwningAbility, TaskInstanceName);
	return MyObj;
}
 
void UAbilityTask_TickTask::Activate()
{
	Super::Activate();
}
 
void UAbilityTask_TickTask::TickTask(const float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}
