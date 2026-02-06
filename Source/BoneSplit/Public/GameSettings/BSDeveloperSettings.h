// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Actors/Player/BSSaveGame.h"
#include "BoneSplit/BoneSplit.h"
#include "Engine/DeveloperSettings.h"
#include "GameInstance/BSCalendarEventSubsystem.h"
#include "Widgets/BSLoadingScreenWidget.h"
#include "Widgets/BSWHud.h"
#include "Widgets/BSWRoot.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Widgets/CharacterWidgets/BSCharacterPane.h"
#include "Widgets/CharacterWidgets/BSPauseMenu.h"
#include "Widgets/BSWToolTipBase.h"
#include "Widgets/CharacterWidgets/BSWDamageNumber.h"
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
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Data")
	FGameplayTagContainer SavedGameplayTags = FGameplayTagContainer::CreateFromArray( 
		TArray{ BSTags::Talent_Jump.GetTag().RequestDirectParent() });
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Player")
	TArray<FColor> PlayerColors;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Widgets")
	TSoftClassPtr<UBSWRoot> RootWidgetClass;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Widgets")
	TSoftClassPtr<UBSWHud> HudWidgetClass;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Widgets")
	TSoftClassPtr<UBSCharacterPane> CharacterPaneWidgetClass;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Widgets")
	TSoftClassPtr<UBSPauseMenu> PauseMenuWidgetClass;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Widgets")
	TSoftClassPtr<UBSWToolTipBase> DefaultTooltipWidgetClass;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Widgets")
	TSoftClassPtr<UBSWDamageNumber> DefaultDamageNumberWidgetClass;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Widgets")
	TSoftClassPtr<UBSLoadingScreenWidget> LoadingScreenWidget;
	
};
