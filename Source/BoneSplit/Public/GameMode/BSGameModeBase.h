// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BSGameModeBase.generated.h"

class ABSMobSpawner;
class ABSMobCharacter;
/**
 * 
 */
UCLASS()
class BONESPLIT_API ABSGameModeBase : public AGameMode
{
	GENERATED_BODY()
	
public:
	
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = L"") override;
};
