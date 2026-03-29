// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BoneSplit/BoneSplit.h"
#include "Engine/DeveloperSettings.h"
#include "GameInstance/BSCalendarEventSubsystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Widgets/Base/BSWToolTipBase.h"
#include "BSDeveloperSettings.generated.h"

class UBSWidget_UnitPlate;
/**
 * 
 */
UCLASS(DefaultConfig, Config=Game, Blueprintable, BlueprintType)
class BONESPLIT_API UBSDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	
	static const UBSDeveloperSettings* GetProjectSettings() { return GetDefault<UBSDeveloperSettings>(); }

#if WITH_EDITOR
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;

	static int32 GetDaysInMonth(int32 Month);

#endif
	
	virtual FName GetCategoryName() const override { return FName("Bone Split"); }
	
	UPROPERTY(Config, EditDefaultsOnly, Category="CalendarEvents")
	TArray<FBSSeasonalEvent> SeasonalEvents;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Mobs")
	TSoftClassPtr<UBSWidget_UnitPlate> DefaultUnitPlate;
	
	//Used to convert a conventional gameplay tag into an in game UI name
	UPROPERTY(Config, EditDefaultsOnly, Category="Tags")
	TMap<FGameplayTag, FText> TagReadableNames;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Player")
	TArray<FColor> PlayerColors;                           
};
