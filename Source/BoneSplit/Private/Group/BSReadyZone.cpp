// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Group/BSReadyZone.h"

#include "Group/BSGroupSubsystem.h"

ABSReadyZone::ABSReadyZone()
{
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
}

void ABSReadyZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (bWasTriggered) return;
	
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (!PlayerPawn || !PlayerPawn->IsPlayerControlled()) return;
	
	UBSGroupSubsystem* GroupSubsystem = UBSGroupSubsystem::Get(GetWorld());
	if (!GroupSubsystem) return;
	
	GroupSubsystem->SetPlayerReady(PlayerPawn, true);
	
	if (GroupSubsystem->IsAllPlayersReady())
	{
		GroupSubsystem->FlushReadyPlayers();
		bWasTriggered = true;
		
		if (OnAllPlayersReadyDelegate.IsBound())
		{
			OnAllPlayersReadyDelegate.Broadcast();
		}
	}
}

void ABSReadyZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	
	if (bWasTriggered) return;
	
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (!PlayerPawn || !PlayerPawn->IsPlayerControlled()) return;
	
	UBSGroupSubsystem* GroupSubsystem = UBSGroupSubsystem::Get(GetWorld());
	if (!GroupSubsystem) return;
	
	GroupSubsystem->SetPlayerReady(PlayerPawn, false);
}

