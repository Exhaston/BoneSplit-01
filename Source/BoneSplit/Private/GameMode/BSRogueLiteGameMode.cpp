// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "BoneSplit/Public/GameMode/BSRogueLiteGameMode.h"

#include "AdvancedGameSession.h"

ABSRogueLiteGameMode::ABSRogueLiteGameMode(const FObjectInitializer& Initializer) : Super(Initializer)
{
	GameSessionClass = AAdvancedGameSession::StaticClass();
}
