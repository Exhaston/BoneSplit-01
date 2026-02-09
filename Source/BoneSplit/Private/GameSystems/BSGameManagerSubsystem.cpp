// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameSystems/BSGameManagerSubsystem.h"


void UBSGameManagerSubsystem::StartLoadMap(const TSoftObjectPtr<UWorld> InMapAsset, const FString InPlayerSpawnTag)
{
	if (!GetGameInstance()) return;
	const FString TravelDest = InMapAsset.GetAssetName();

	/*
	if (!GetWorld()->IsNetMode(NM_ListenServer) && GetPartyMode() != EBSPartyMode::Party_Private)
	{
		TravelDest += "?Listen";
	}
	*/
	
	SetTravelTagDestination(InPlayerSpawnTag);
	
	GetGameInstance()->GetWorld()->ServerTravel(TravelDest);
}
