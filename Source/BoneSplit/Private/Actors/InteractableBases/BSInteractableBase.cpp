// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/InteractableBases/BSInteractableBase.h"


// Sets default values
ABSInteractableBase::ABSInteractableBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABSInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABSInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

