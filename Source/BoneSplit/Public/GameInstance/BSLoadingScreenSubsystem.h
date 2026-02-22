// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSLoadingScreenSubsystem.generated.h"

class UBSLoadingScreenWidget;
class UBSMapData;

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnWorldTravel, const UBSMapData* MapData);

UCLASS()
class BONESPLIT_API UBSLoadingScreenSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	UBSLoadingScreenSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	//Called from the travel manager when traveling first occurs.
	virtual void AddLoadingScreen(APlayerController* LocalController);
	
	//Called from a pawn or controller when the client is fully setup.
	virtual void RemoveLoadingScreen(APlayerController* LocalController);
	
	FBSOnWorldTravel OnMapLoadBegin;
	
	FSimpleMulticastDelegate OnMapLoadEnd;
	
protected:
	
	//Only valid on authority.
	UPROPERTY()
	FString CurrentTravelTag;
	
	TSharedPtr<SWidget> LoadingScreenWidgetIns;
};
