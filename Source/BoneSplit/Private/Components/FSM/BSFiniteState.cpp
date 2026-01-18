// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/FSM//BSFiniteState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/FSM/BSFiniteStateComponent.h"

void UBSFiniteState::NativeEnterState(UBSFiniteStateComponent* InOwnerComponent)
{
	OwnerComponent = InOwnerComponent;
	BP_OnEnterState(InOwnerComponent);
	bCanTick = true;
}

void UBSFiniteState::NativeTickState(UBSFiniteStateComponent* InOwnerComponent, const float DeltaTime)
{
	if (!bCanTick) return;
	ElapsedTickTime += DeltaTime;
	if (ElapsedTickTime >= TickInterval)
	{
		ElapsedTickTime = 0;
		BP_OnTickState(InOwnerComponent, DeltaTime);
	}
}

void UBSFiniteState::NativeEndState(UBSFiniteStateComponent* InOwnerComponent)
{
	bCanTick = false;
	BP_OnEndState(InOwnerComponent);
}

UWorld* UBSFiniteState::GetWorld() const
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		return GetOuter()->GetWorld();
	}
	return nullptr;
}

void UBSFiniteState::ChangeState(const TSubclassOf<UBSFiniteState> NewState) const
{
	OwnerComponent->ChangeState(NewState);
}

UBSFiniteStateComponent* UBSFiniteState::GetOwnerComponent() const
{
	return OwnerComponent;
}

AActor* UBSFiniteState::GetOwnerActor() const
{
	return OwnerComponent->GetOwner();
}

bool UBSFiniteState::GetShouldPool() const
{
	return bPooled;
}

void UBSFiniteState::BP_OnEndState_Implementation(UBSFiniteStateComponent* InOwnerComponent)
{
}

void UBSFiniteState::BP_OnTickState_Implementation(UBSFiniteStateComponent* InOwnerComponent, float DeltaTime)
{
}

void UBSFiniteState::BP_OnEnterState_Implementation(UBSFiniteStateComponent* InOwnerComponent)
{
}
