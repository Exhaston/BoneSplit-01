// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSGameManagerSubsystem.generated.h"

UCLASS()
class BONESPLIT_API UBSGameManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	virtual void StartLoadMap(TSoftObjectPtr<UWorld> InMapAsset, FString InPlayerSpawnTag);
	
	//Gets the current travel destination tag for deciding which player start to choose.
	FString GetTravelDestinationTag() { return TravelDestinationTag; }

protected:
	
	//Overrides the current Travel Destination tag before travel. 
	//Access the tag from a game mode to spawn the player in the new player start with the same tag.
	void SetTravelTagDestination(FString NewTravelDestinationTag) { TravelDestinationTag = NewTravelDestinationTag; }
	
	UPROPERTY()
	FString TravelDestinationTag;
};

