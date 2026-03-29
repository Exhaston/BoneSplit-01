// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "LockedActors/BSDoor.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ABSDoor::ABSDoor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetNetUpdateFrequency(8);
	bAlwaysRelevant = true;
}

void ABSDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSDoor, bIsLocked);
}

void ABSDoor::OnRep_LockDoor()
{
	if (bIsLocked)
	{
		OnLocked();
	}
	else
	{
		OnUnlocked();
	}
}

void ABSDoor::OnLocked_Implementation()
{
}

void ABSDoor::OnUnlocked_Implementation()
{
}

// Called when the game starts or when spawned
void ABSDoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority()) return;

	// Only apply default state if nobody has explicitly set us yet
	if (!bLockStateSetExternally)
	{
		SetLockState(bStartLocked);
	}
}

void ABSDoor::SetLockState(bool bLocked)
{
	if (!HasAuthority()) return;
	
	bLockStateSetExternally = true;
	bIsLocked = bLocked;
	if (bLocked)
	{
		OnLocked();
	}
	else
	{
		OnUnlocked();
	}
}

