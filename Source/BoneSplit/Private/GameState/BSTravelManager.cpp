// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameState/BSTravelManager.h"

#include "GameFramework/GameStateBase.h"
#include "GameInstance/BSPersistantDataSubsystem.h"
#include "Net/UnrealNetwork.h"


UBSTravelManager::UBSTravelManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UBSTravelManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBSTravelManager, ReadyPlayers);
}

void UBSTravelManager::Server_TravelStart(const TSubclassOf<UBSMapData> MapData, FString TravelDestTag)
{
	if (GetOwner()->HasAuthority())
	{
		Multicast_TravelStart(MapData);
		
		UBSPersistantDataSubsystem* PersistantSubsystem = UBSPersistantDataSubsystem::Get(GetOwner());
		PersistantSubsystem->StartTravel(MapData, TravelDestTag);
	}
}

void UBSTravelManager::OnRep_ReadyPlayers()
{
}

void UBSTravelManager::OnRep_PlayersReady()
{
}

void UBSTravelManager::Multicast_TravelStart_Implementation(const TSubclassOf<UBSMapData> MapData)
{
	const UBSMapData* MapDataCDO = GetDefault<UBSMapData>(MapData);
	
	UBSPersistantDataSubsystem* GameManagerSubsystem = UBSPersistantDataSubsystem::Get(GetOwner());
	GameManagerSubsystem->AddLoadingScreen(MapDataCDO);
}

UBSTravelManager* UBSTravelManager::GetTravelManager(const UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	const AGameStateBase* GS = World->GetGameStateChecked<AGameStateBase>();
	return GS->GetComponentByClass<UBSTravelManager>();
}

