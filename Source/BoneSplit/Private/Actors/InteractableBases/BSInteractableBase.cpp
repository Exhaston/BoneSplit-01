// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/InteractableBases/BSInteractableBase.h"

ABSInteractableBase::ABSInteractableBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

