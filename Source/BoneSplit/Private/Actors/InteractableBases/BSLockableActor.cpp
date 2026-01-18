// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/InteractableBases/BSLockableActor.h"


ABSLockableActor::ABSLockableActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABSLockableActor::NativeOnLocked()
{
	bLocked = true;
	BP_OnLocked();
}

void ABSLockableActor::NativeOnUnLocked()
{
	bLocked = false;
	BP_OnUnlocked();
}

bool ABSLockableActor::IsLocked() const
{
	return bLocked;
}

void ABSLockableActor::BP_OnLocked_Implementation()
{
}

void ABSLockableActor::BP_OnUnlocked_Implementation()
{
}

