// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "BoneSplit/Public/GameState/BSGameState.h"

#include "Actors/Player/BSPlayerState.h"
#include "Kismet/GameplayStatics.h"

ABSGameState::ABSGameState(const FObjectInitializer& Initializer) : Super(Initializer)
{
	
}

void ABSGameState::Server_ReleasePauseRequest(APlayerState* PS)
{
	PauseRequestedPlayers.Remove(PS);
	UGameplayStatics::SetGamePaused(this, false);
}

void ABSGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	Server_ReleasePauseRequest(PlayerState);
}

void ABSGameState::Server_RequestPause(APlayerState* PS)
{
	PauseRequestedPlayers.AddUnique(PS);
	if (PauseRequestedPlayers.Num() == PlayerArray.Num())
	{
		UGameplayStatics::SetGamePaused(this, true);
	}
	
}
