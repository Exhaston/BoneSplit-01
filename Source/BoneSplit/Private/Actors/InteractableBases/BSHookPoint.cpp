// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/InteractableBases/BSHookPoint.h"


ABSHookPoint::ABSHookPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	HookMesh->SetupAttachment(GetRootComponent());
	EndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPoint"));
	EndPoint->SetupAttachment(GetRootComponent());
}

FVector ABSHookPoint::GetTargetLocation(AActor* RequestedBy) const
{
	return EndPoint->GetComponentLocation();
}

