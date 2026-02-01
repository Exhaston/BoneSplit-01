// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSLoadingScreenSubsystem.generated.h"

class UBSLoadingScreenWidget;
class UBSMapData;

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnWorldTravel, const UBSMapData* MapData);

UCLASS()
class BONESPLIT_API UBSLoadingScreenSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	
	UBSLoadingScreenSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	//See travel manager for proper traveling. 
	//This has to exist only to store persistant travel data like the next spawn point. 
	virtual void StartTravel(const TSubclassOf<UBSMapData> MapData, FString TravelDestTag);
	
	//Called from the travel manager when traveling first occurs.
	virtual void AddLoadingScreen(const UBSMapData* MapData);
	
	//Called from a pawn or controller when the client is fully setup.
	virtual void RemoveLoadingScreen();
	
	FBSOnWorldTravel OnMapLoadBegin;
	
	FSimpleMulticastDelegate OnMapLoadEnd;
	
protected:
	
	//Only valid on authority.
	UPROPERTY()
	FString CurrentTravelTag;
	
	TSharedPtr<SWidget> LoadingScreenWidgetIns;
};
