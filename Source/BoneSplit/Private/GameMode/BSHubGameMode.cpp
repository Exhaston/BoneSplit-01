// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameMode/BSHubGameMode.h"

#include "AdvancedGameSession.h"

ABSHubGameMode::ABSHubGameMode(const FObjectInitializer& Initializer) : Super(Initializer)
{
	GameSessionClass = AAdvancedGameSession::StaticClass();
}
