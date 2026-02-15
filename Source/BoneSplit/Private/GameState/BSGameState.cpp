// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "BoneSplit/Public/GameState/BSGameState.h"

#include "Actors/Player/BSPlayerState.h"
#include "GameState/BSTravelManager.h"
#include "Kismet/GameplayStatics.h"

ABSGameState::ABSGameState(const FObjectInitializer& Initializer) : Super(Initializer)
{
	TravelManagerComponent = CreateDefaultSubobject<UBSTravelManager>(TEXT("TravelManagerComponent"));
}

void ABSGameState::SpawnEquipmentForPlayer(ABSPlayerState* PS, const TSubclassOf<UBSEquipmentEffect> Effect)
{
	FBSLootSpawnInfo LootSpawnInfo;
	LootSpawnInfo.LootGuid = FGuid::NewGuid();
	LootSpawnInfo.EquipmentEffect = Effect;
	//PS->Client_SpawnEquipmentLoot(LootSpawnInfo);
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
