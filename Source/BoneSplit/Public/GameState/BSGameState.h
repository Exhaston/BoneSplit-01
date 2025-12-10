// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BSGameState.generated.h"

class UBSTravelManager;
/**
 * 
 */
UCLASS()
class BONESPLIT_API ABSGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
	explicit ABSGameState(const FObjectInitializer& Initializer);
	
	UPROPERTY()
	TObjectPtr<UBSTravelManager> TravelManagerComponent;
};
