// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameSystems/BSGameMode.h"

#include "AdvancedGameSession.h"
#include "GameSystems/BSGameManagerSubsystem.h"

ABSGameMode::ABSGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GameSessionClass = AAdvancedGameSession::StaticClass();
	bUseSeamlessTravel = true;
}

void ABSGameMode::BP_LoadMap(const TSoftObjectPtr<UWorld> InMap, const FString NextPlayerStartTag) const
{
	if (!ensure(!InMap.IsNull())) return;
	if (UBSGameManagerSubsystem* GameManagerSubsystem = GetGameInstance()->GetSubsystem<UBSGameManagerSubsystem>())
	{
		GameManagerSubsystem->TravelToMap(InMap, NextPlayerStartTag);
	}
}

AActor* ABSGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	FString Dest = IncomingName;
	if (UBSGameManagerSubsystem* GameManagerSubsystem = GetGameInstance()->GetSubsystem<UBSGameManagerSubsystem>())
	{
		Dest = GameManagerSubsystem->GetTravelDestinationTag();
		return Super::FindPlayerStart_Implementation(Player, Dest);
	}
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}
