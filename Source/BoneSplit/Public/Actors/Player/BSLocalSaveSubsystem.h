// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSLocalSaveSubsystem.generated.h"

class UAbilitySystemComponent;
struct FBSSaveData;
class UBSSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBSAsyncSaveDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSAsyncLoadDelegate, UBSSaveGame*, LoadedSave);
/**
 * 
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class BONESPLIT_API UBSLocalSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	//Gets the CDO of the Save Game class. Useful to get default initialization data. 
	//Do not save this, as it would be pointless
	const UBSSaveGame* GetSaveGameCDO() const;
	
	//Returns the correct BP subclass, as defined in developer settings.
	TSubclassOf<UBSSaveGame> GetSaveGameClass();
	
	//Gets all the available 'profile' names. Finds the save files in the folder.
	static bool GetSaveGameNames(TArray<FString>& FoundSaves, const int32 UserIndex);
	
	//Gets or creates a new save game instance. This is what holds savable mutable data. This can be saved.
	UBSSaveGame* GetOrCreateSaveGameInstance(const APlayerController* PlayerController);
	
	//Gets the current save game instance save data. This is the actual information being saved about the player.
	FBSSaveData& GetOrCreateSaveData(const APlayerController* PlayerController);
	
	//Loads the game async. OnAsyncLoadComplete delegate broadcasts when complete.
	void LoadGameAsync(const APlayerController* PlayerController);
	
	//Saves the game instantly and might halt the game thread if heavy load. does not fire async delegates.
	virtual void SaveAscDataSync(APlayerController* PlayerController, UAbilitySystemComponent* Asc);
	
	//Loads the game async. OnAsyncSaveComplete delegate broadcasts when complete.
	virtual void SaveAscDataAsync(APlayerController* PlayerController, UAbilitySystemComponent* Asc);
	
	//Executed on the server to apply default Asc data from the save file.
	virtual void ApplyDefaultData(UAbilitySystemComponent* Asc);
	
	//Executed on server to restore the Save data to the asc. 
	//RPC the save data from the client to server to achieve MP save support.
	static void PopulateAscFromSaveData(UAbilitySystemComponent* Asc, const FBSSaveData& Data);
	
	FBSAsyncSaveDelegate& GetOnAsyncSaveCompleteDelegate() { return OnAsyncSaveComplete; }
	
	FBSAsyncLoadDelegate& GetOnAsyncLoadCompleteDelegate() { return OnAsyncLoadComplete; }
	
protected:
	
	FBSAsyncSaveDelegate OnAsyncSaveComplete;
	
	FBSAsyncLoadDelegate OnAsyncLoadComplete;
	
	virtual void SaveAscData(UAbilitySystemComponent* Asc, FBSSaveData& Data);
	
	void SaveGameSync(const APlayerController* PlayerController) const;
	
	void SaveGameAsync(const APlayerController* PlayerController);
	
	UPROPERTY()
	TObjectPtr<UBSSaveGame> SaveGameInstance;
	
	UPROPERTY()
	TSubclassOf<UBSSaveGame> SaveGameClass;
	
	UPROPERTY()
	FString CurrentProfile = "DefaultSave";
};
