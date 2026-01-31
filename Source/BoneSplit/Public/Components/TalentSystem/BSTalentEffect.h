// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "IconThumbnailInterface.h"
#include "UObject/Object.h"
#include "BSTalentEffect.generated.h"

class UBSTalentEffect;
class UGameplayEffect;

USTRUCT(BlueprintType, Blueprintable)
struct FBSTalentSaveData
{
	GENERATED_BODY()
	
	UPROPERTY(SaveGame)
	int32 CurrentLevel = 1;
	
	UPROPERTY(SaveGame)
	TSubclassOf<UBSTalentEffect> TalentClass;
};

/**
 * Like gameplay abilities, this is an uobject instead of a data asset, 
 * as it's easier and faster to sync these over the network.
 */
UCLASS(Blueprintable, BlueprintType, DisplayName="Talent Effect", HideCategories=("Duration"))
class BONESPLIT_API UBSTalentEffect : public UGameplayEffect, public IIconThumbnailInterface
{
	GENERATED_BODY()
	
public:
	
	UBSTalentEffect();
	
#if WITH_EDITOR
	
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	
#endif
	     
	//What icon to be loaded in the interface overviewing all the talents.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UTexture2D> TalentIcon;
	                          
	//UI Name of the talent
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText TalentName;
	
	//What does the talent do, this will be displayed in the UI elements
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText Description;
	
	//Get the maximum number this talent can be gained, based on effect stack limit
	int32 GetMaxLevel() const;
	
	//All Talents required to be active before this is applicable/Available
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<UBSTalentEffect>> RequiredTalents;
	
	//Any of these talents required to be available
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<UBSTalentEffect>> AnyRequiredTalents;
	
	virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation() const override;
	virtual FLinearColor GetTint_Implementation() const override;
};
