// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameMode/BSGameModeBase.h"

#include "GameInstance/BSLoadingScreenSubsystem.h"

AActor* ABSGameModeBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
	//return Super::FindPlayerStart_Implementation(
	//	Player, 
	//	UBSPersistantDataSubsystem::Get(this)->GetTravelDestTag());
}
