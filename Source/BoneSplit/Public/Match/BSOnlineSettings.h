// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BSOnlineSettings.generated.h"

class UBSOnlineSettings;

static const UBSOnlineSettings* GetBSOnlineSettings() { return GetDefault<UBSOnlineSettings>(); }

/**
 * 
 */
UCLASS(DefaultConfig, Config=Game, Blueprintable, BlueprintType)
class BONESPLIT_API UBSOnlineSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	
	virtual FName GetCategoryName() const override { return FName("BoneSplit"); }
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Audio")
	TSoftObjectPtr<USoundClass> MasterSoundClass; 
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Audio")
	TSoftObjectPtr<USoundClass> EffectsSoundClass;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Audio")
	TSoftObjectPtr<USoundClass> MusicSoundClass;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Match")
	TSoftObjectPtr<UWorld> MainMenuMap;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Match")
	TSoftObjectPtr<UWorld> LobbyMap;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Match")
	TSoftObjectPtr<UWorld> GameStartMap;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Multiplayer")
	int32 MaxPlayers = 4;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Multiplayer", meta=(Units=Seconds, ClampMin=0, ClampMax=120))
	float OnlineRequestTimeOut = 30;
};
