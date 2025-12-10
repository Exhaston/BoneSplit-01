// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "BoneSplit/Public/GameState/BSGameState.h"

#include "GameState/BSTravelManager.h"

ABSGameState::ABSGameState(const FObjectInitializer& Initializer) : Super(Initializer)
{
	TravelManagerComponent = CreateDefaultSubobject<UBSTravelManager>(TEXT("TravelManagerComponent"));
}
