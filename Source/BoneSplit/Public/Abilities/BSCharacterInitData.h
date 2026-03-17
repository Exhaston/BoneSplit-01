// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefaultAttributeSet.h"
#include "IconThumbnailInterface.h"
#include "Engine/DataAsset.h"
#include "BSCharacterInitData.generated.h"

class UDefaultAttributeSet;
class UGameplayEffect;
struct FGameplayAttribute;
class UCharacterAbilityBase;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Const, DisplayName="Character Initialization Data")
class BONESPLIT_API UBSCharacterInitData : public UPrimaryDataAsset, public IIconThumbnailInterface
{
	GENERATED_BODY()
	
public:
	
	UBSCharacterInitData();
	
	virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation() const override;
	virtual FLinearColor GetTint_Implementation() const override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UTexture2D> CharacterIcon;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FLinearColor IconTint = FLinearColor::White;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText CharacterName = FText::FromString("Nameless");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="Faction."))
	FGameplayTagContainer FactionTags;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0))
	int32 StartLevel = 0;
	
	//For mobs this would be any abilities it can use. For the player it should be static abilities the player always has, such as jumping, interacting etc.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<UCharacterAbilityBase>> GrantedDefaultAbilities;
	
	//Mostly used for mobs. Should be used for passive effects that persist at all times. Examples: Mana regen, aura etc.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<UGameplayEffect>> GrantedEffects;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, DisplayName="Attributes", meta=(ForceInlineRow))
	TMap<FGameplayAttribute, FScalableFloat> AttributeMap;
};
