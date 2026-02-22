// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Components/Inventory/BSEquipment.h"
#include "Components/TalentSystem/BSTalentEffect.h"
#include "GameFramework/SaveGame.h"
#include "BSSaveGame.generated.h"

class UBSEquipmentEffect;
class UBSTalentEffect;
class UGameplayAbility;
class UGameplayEffect;

//Minimal storable effect data.
USTRUCT(BlueprintType)
struct FBSGameplayEffectData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(SaveGame)
	float RemainingTime = 0;

	UPROPERTY(SaveGame)
	int32 StackCount = 1;
};

/*
 * Represents an attribute as a minimal storable format. Contains the attribute struct and its current BASE (Without stateful modifiers) value.
 */
USTRUCT(BlueprintType)
struct FBSAttributeData
{
	GENERATED_BODY()

	FBSAttributeData() = default;
	
	FBSAttributeData(const FGameplayAttribute& InAttribute, const float InValue)
	{
		Attribute = InAttribute;
		Value = InValue;
	}
		
	UPROPERTY(EditAnywhere, SaveGame)
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, meta=(ClampMin=0), SaveGame)
	float Value = 0;

	bool operator==(const FBSAttributeData& Other) const
	{
		return Attribute == Other.Attribute;
	}
};

//Data only struct that contains everything a player needs saved
USTRUCT(Blueprintable, BlueprintType)
struct FBSSaveData
{
	GENERATED_BODY()
	
	//Stored Attributes. Clear in hub.
	UPROPERTY(SaveGame)
	TArray<FBSAttributeData> Attributes;
	
	//Current effects, needed for transitions between maps with buffs etc. Clear in hub. 
	UPROPERTY(SaveGame)
	TArray<FBSGameplayEffectData> Effects;
	
	//Current selected color. This is just an identifier for a color array of available colors.
	UPROPERTY(SaveGame, BlueprintReadOnly, EditDefaultsOnly)
	int32 CurrentColor = 0;
	
	UPROPERTY(SaveGame)
	TArray<FBSTalentSaveData> GrantedTalents;
};

/**
 * Save Game Base class that holds default initialization information, and have SaveData that can be serialized and saved.
 * Save this object with GameplayStatics save functionality.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DisplayName="Save Game Base", Category="BoneSplit")
class BONESPLIT_API UBSSaveGame : public USaveGame
{
	GENERATED_BODY()
	                                     
public:
	
	//Save data stored within a struct for server RPC convenience
	UPROPERTY(SaveGame, BlueprintReadOnly, EditDefaultsOnly)
	FBSSaveData SaveData;
	                                                                                                           
	//Default effects any player has regardless of gear. 
	//Default attributes and other persistent effect are good to have here.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Initialization Data")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Initialization Data")
	TArray<TSubclassOf<UBSEquipmentEffect>> DefaultEquipment;
	
	//Just here for testing purposes. Will be deprecated when equipment is fully implemented
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Initialization Data")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
	
	//Default tags such as progression, unlocks and factions.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Initialization Data")
	TMap<FGameplayTag, int32> DefaultTags;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Initialization Data")
	TArray<TSubclassOf<UGameplayEffect>> SaveableEffectClasses = {
		UBSEquipmentEffect::StaticClass(), 
		UBSTalentEffect::StaticClass()
	}; 
};
