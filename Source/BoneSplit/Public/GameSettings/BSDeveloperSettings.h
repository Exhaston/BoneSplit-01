// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Actors/Player/BSSaveGame.h"
#include "Engine/DeveloperSettings.h"
#include "GameInstance/BSCalendarEventSubsystem.h"
#include "Widgets/BSLoadingScreenWidget.h"
#include "BSDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(DefaultConfig, Config=Game, Blueprintable, BlueprintType)
class BONESPLIT_API UBSDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	
#if WITH_EDITOR
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;

	static int32 GetDaysInMonth(int32 Month);

#endif
	
	virtual FName GetCategoryName() const override { return FName("Bone Split"); }
	
	UPROPERTY(Config, EditDefaultsOnly, Category="CalendarEvents")
	TArray<FBSSeasonalEvent> SeasonalEvents;
	
	//Used to convert a conventional gameplay tag into an in game UI name
	UPROPERTY(Config, EditDefaultsOnly, Category="Tags")
	TMap<FGameplayTag, FText> TagReadableNames;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Data")
	TSoftClassPtr<UBSSaveGame> SaveGameClass;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Player")
	TArray<FColor> PlayerColors;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Widgets")
	TSoftClassPtr<UBSLoadingScreenWidget> LoadingScreenWidget;
};
