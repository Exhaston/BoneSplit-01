// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/FSM/BSFiniteStateComponent.h"

#include "Components/FSM/BSFiniteState.h"


UBSFiniteStateComponent::UBSFiniteStateComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBSFiniteStateComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (CurrentState)
	{
		CurrentState->NativeTickState(this, DeltaTime);
	}
}

void UBSFiniteStateComponent::ChangeState(const TSubclassOf<UBSFiniteState> NewState)
{
	if (CurrentState)
	{
		CurrentState->NativeEndState(this);
		
		if (UWorld* World = GetWorld())
		{
			World->GetLatentActionManager().RemoveActionsForObject(CurrentState);
		}
		
		if (CurrentState->GetShouldPool())
		{
			PooledStates.AddUnique(CurrentState); //Keep alive for next activation
		}
		
		CurrentState = nullptr;
	}
	
	if (NewState)
	{
		bool bFoundInPool = false;
		
		//If the state wasn't poolable it would never have been added
		if (UBSFiniteState** FoundState = PooledStates.FindByPredicate([NewState](const UBSFiniteState* State)
		{
			return State->IsA(NewState);
		}))
		{
			bFoundInPool = true;
			CurrentState = *FoundState;
		}
		
		if (!bFoundInPool) //Create a new state if nothing was found in pool
		{
			CurrentState = NewObject<UBSFiniteState>(this, NewState);
		}
		
		CurrentState->NativeEnterState(this);
	}
}

