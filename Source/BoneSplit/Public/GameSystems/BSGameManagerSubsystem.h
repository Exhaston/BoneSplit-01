// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSGameManagerSubsystem.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActorClass);

class ABSEquipmentDropBase;

UCLASS()
class BONESPLIT_API UBSGameManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	static UBSGameManagerSubsystem* Get(const UObject* WorldContext);
	//Opens Listen server and travels to hub. Called after starting a session or lobby
	virtual void InitializeHost();
	
	virtual void InitializeSinglePlayer();
	//Closes Listen server and returns to MainMenu.
	virtual void QuitToMenu();
	//Intended way to travel between gameplay levels. Uses seamless travel and should only be called from host. 
	//Takes a player spawn tag to decide where to spawn the player in the new level. Matching Player Start spawn tags.
	virtual void TravelToMap(TSoftObjectPtr<UWorld> InMapAsset, FString InPlayerSpawnTag);
	
	UFUNCTION()
	void OnSessionEnd();
	
	//Gets the current travel destination tag for deciding which player start to choose.
	FString GetTravelDestinationTag() { return TravelDestinationTag; }
	
	//Function to get the current blueprint authored widget from project settings.
	template<typename T>
	TSubclassOf<T> GetBlueprintOverrideClass(const FGameplayTag Tag) const
	{
		if (UClass* const* Value = ActorOverrideClasses.Find(Tag))
			return Value;

		return nullptr;
	}

protected:
	
	UPROPERTY()
	TMap<FGameplayTag, UClass*> ActorOverrideClasses;
	
	UPROPERTY()
	TSoftObjectPtr<UWorld> HubLevel;
	
	UPROPERTY()
	TSoftObjectPtr<UWorld> MainMenuLevel;
	
	//Overrides the current Travel Destination tag before travel. 
	//Access the tag from a game mode to spawn the player in the new player start with the same tag.
	void SetTravelTagDestination(FString NewTravelDestinationTag) { TravelDestinationTag = NewTravelDestinationTag; }
	
	UPROPERTY()
	FString TravelDestinationTag;
	

};

