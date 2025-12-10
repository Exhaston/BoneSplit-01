// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BSRogueLiteGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BONESPLIT_API ABSRogueLiteGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	
	explicit ABSRogueLiteGameMode(const FObjectInitializer& Initializer);
	
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = L"") override;
};
