// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "BoneSplit/Public/GameMode/BSRogueLiteGameMode.h"

#include "GameInstance/BSPersistantDataSubsystem.h"

ABSRogueLiteGameMode::ABSRogueLiteGameMode(const FObjectInitializer& Initializer) : Super(Initializer)
{
}

AActor* ABSRogueLiteGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	return Super::FindPlayerStart_Implementation(
		Player, 
		UBSPersistantDataSubsystem::Get(this)->GetTravelDestTag());
}
