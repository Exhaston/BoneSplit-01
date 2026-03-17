// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "LockedActors/BSTreasureChest.h"


// Sets default values
ABSTreasureChest::ABSTreasureChest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABSTreasureChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABSTreasureChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

