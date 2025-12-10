// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BoneSplit/BoneSplit.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSPersistantDataSubsystem.generated.h"

class UBSSaveGame;
class UBSLoadingScreenWidget;
class UBSMapData;

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnWorldTravel, const UBSMapData* MapData);

UCLASS()
class BONESPLIT_API UBSPersistantDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	static UBSPersistantDataSubsystem* Get(const UObject* WorldContext);
	
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
	
	FString GetTravelDestTag() const;
	
	EBSPartyMode GetPartyMode() const;
	
	//Does not work for dedicated servers. Will return the local player controller's game save.
	UBSSaveGame* GetOrLoadSaveGame(const APlayerController* PC);
	
	UPROPERTY()
	TSubclassOf<UBSSaveGame> SaveGameClass;
	
protected: 
	
	UPROPERTY()
	TObjectPtr<UBSSaveGame> SaveGameInstance;
	
	//Only valid on authority.
	UPROPERTY()
	FString CurrentTravelTag;
	
	//Only valid on authority.
	UPROPERTY()
	EBSPartyMode PartyMode = EBSPartyMode::Party_Open;
	
	TSharedPtr<SWidget> LoadingScreenWidgetIns;
};
