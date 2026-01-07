// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameMode/BSHubGameMode.h"

#include "AdvancedGameSession.h"
#include "GameInstance/BSPersistantDataSubsystem.h"

ABSHubGameMode::ABSHubGameMode(const FObjectInitializer& Initializer) : Super(Initializer)
{
	GameSessionClass = AAdvancedGameSession::StaticClass();
}

AActor* ABSHubGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	return Super::FindPlayerStart_Implementation(
		Player, 
		UBSPersistantDataSubsystem::Get(this)->GetTravelDestTag());
}
