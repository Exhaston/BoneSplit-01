// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "BoneSplit/Public/GameState/BSGameState.h"
#include "GameState/BSTravelManager.h"
#include "Kismet/GameplayStatics.h"

ABSGameState::ABSGameState(const FObjectInitializer& Initializer) : Super(Initializer)
{
	TravelManagerComponent = CreateDefaultSubobject<UBSTravelManager>(TEXT("TravelManagerComponent"));
}

void ABSGameState::Server_ReleasePauseRequest(APlayerState* PS)
{
	PauseRequestedPlayers.Remove(PS);
	UGameplayStatics::SetGamePaused(this, false);
}

void ABSGameState::Server_RequestPause(APlayerState* PS)
{
	PauseRequestedPlayers.AddUnique(PS);
	if (PauseRequestedPlayers.Num() == PlayerArray.Num())
	{
		UGameplayStatics::SetGamePaused(this, true);
	}
	
}
