// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameMode/BSMainMenuGameMode.h"

#include "AdvancedGameSession.h"
#include "Widgets/MainMenu/BSMenuController.h"


ABSMainMenuGameMode::ABSMainMenuGameMode(const FObjectInitializer& Initializer) : Super(Initializer)
{
	GameSessionClass = AAdvancedGameSession::StaticClass();
	PlayerControllerClass = ABSMenuController::StaticClass();
}
