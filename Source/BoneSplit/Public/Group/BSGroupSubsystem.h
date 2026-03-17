// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BSGroupSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnPlayersReady, TArray<APawn*> PlayerPawns);

UCLASS()
class BONESPLIT_API UBSGroupSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	static UBSGroupSubsystem* Get(const UWorld* World);
	
	virtual void SetPlayerReady(APawn* PlayerPawn, bool bReady);
	
	virtual void FlushReadyPlayers();
	
	virtual bool IsAllPlayersReady();
	
	FBSOnPlayersReady& GetOnPlayersReadyDelegate();
	
protected:
	
	FBSOnPlayersReady OnPlayersReadyDelegate;
	
	UPROPERTY()
	TArray<APawn*> ReadyPlayers;
};
