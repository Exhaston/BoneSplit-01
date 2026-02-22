// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BSGameState.generated.h"

class ABSPlayerState;
class UGameplayEffect;
class UBSEquipmentEffect;
class UBSTravelManager;



UCLASS()
class BONESPLIT_API ABSGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
	explicit ABSGameState(const FObjectInitializer& Initializer);
	
	void Server_RequestPause(APlayerState* PS);
	
	void Server_ReleasePauseRequest(APlayerState* PS);
	
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	
	UPROPERTY()
	TArray<APlayerState*> PauseRequestedPlayers;
	
};
