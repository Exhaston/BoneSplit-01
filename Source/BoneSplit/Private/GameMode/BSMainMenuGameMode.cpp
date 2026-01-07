// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameMode/BSMainMenuGameMode.h"

#include "AdvancedGameSession.h"


ABSMainMenuGameMode::ABSMainMenuGameMode(const FObjectInitializer& Initializer) : Super(Initializer)
{
	GameSessionClass = AAdvancedGameSession::StaticClass();
}
