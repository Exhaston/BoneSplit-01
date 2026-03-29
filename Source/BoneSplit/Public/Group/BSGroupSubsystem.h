// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BSGroupSubsystem.generated.h"

class UAbilitySystemComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnPlayersReady, TArray<APawn*> PlayerPawns);

DECLARE_MULTICAST_DELEGATE_TwoParams(FBSOnPlayerJoined, ACharacter* InCharacter, UAbilitySystemComponent* PlayerAsc);

UCLASS()
class BONESPLIT_API UBSGroupSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	static UBSGroupSubsystem* Get(const UWorld* World);
	
	virtual void AddPlayerToGroup(ACharacter* InCharacter, UAbilitySystemComponent* PlayerAsc);
	
	virtual void SetPlayerReady(APawn* PlayerPawn, bool bReady);
	
	virtual void FlushReadyPlayers();
	
	virtual bool IsAllPlayersReady();
	
	FBSOnPlayersReady& GetOnPlayersReadyDelegate();
	
	FBSOnPlayerJoined& GetOnPlayerJoinedDelegate() { return OnPlayerJoinedDelegate; }
	
protected:
	
	FBSOnPlayerJoined OnPlayerJoinedDelegate;
	
	FBSOnPlayersReady OnPlayersReadyDelegate;
	
	UPROPERTY()
	TArray<ACharacter*> JoinedPlayers;
	
	UPROPERTY()
	TArray<APawn*> ReadyPlayers;
};
