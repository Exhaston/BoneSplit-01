// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BSGameState.generated.h"

class ABSPlayerState;
class UGameplayEffect;
class UBSEquipmentEffect;
class UBSTravelManager;

//Struct to hold data on what loot has been instanced to clients and what is actually valid to give a client on request.
USTRUCT(BlueprintType)
struct FBSLootSpawnInfo
{
	GENERATED_BODY()
	
	FBSLootSpawnInfo() = default;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(IgnoreForMemberInitializationTest))
	FGuid LootGuid = FGuid::NewGuid();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSEquipmentEffect> EquipmentEffect;
	
	bool operator==(const FBSLootSpawnInfo& Other) const
	{
		return LootGuid == Other.LootGuid;
	}
};

UCLASS()
class BONESPLIT_API ABSGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
	explicit ABSGameState(const FObjectInitializer& Initializer);
	
	virtual void SpawnEquipmentForPlayer(ABSPlayerState* PS, TSubclassOf<UBSEquipmentEffect> Effect);
	
	void Server_RequestPause(APlayerState* PS);
	
	void Server_ReleasePauseRequest(APlayerState* PS);
	
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	
	UPROPERTY()
	TArray<APlayerState*> PauseRequestedPlayers;
	
	UPROPERTY()
	TObjectPtr<UBSTravelManager> TravelManagerComponent;
};
