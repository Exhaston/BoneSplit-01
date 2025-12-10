// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSTravelManager.generated.h"

UCLASS(Blueprintable, BlueprintType, DisplayName="Map Data")
class UBSMapData : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UWorld> Map;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FText MapName;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> MapSplashTexture;
};


UCLASS(ClassGroup=(Custom))
class BONESPLIT_API UBSTravelManager : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSTravelManager();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	virtual void Server_TravelStart(const TSubclassOf<UBSMapData> MapData, FString TravelDestTag);
	
	UPROPERTY(ReplicatedUsing=OnRep_ReadyPlayers)
	TArray<APawn*> ReadyPlayers;
	
	UFUNCTION()
	void OnRep_ReadyPlayers();
	
	UFUNCTION()
	virtual void OnRep_PlayersReady();
	
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContext"))
	static UBSTravelManager* GetTravelManager(const UObject* WorldContext);
	
protected:
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_TravelStart(TSubclassOf<UBSMapData> MapData);
	
};
