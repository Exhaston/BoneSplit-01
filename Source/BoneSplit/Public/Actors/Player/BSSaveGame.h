// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "BSSaveGame.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UBSEquipment;
struct FBSEquipmentInstance;

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
	                                          
	//Saved tags, this can be all from state to story progression
	UPROPERTY(SaveGame)      
	TArray<FGameplayTag> Tags;
	
	//Current selected color. This is just an identifier for a color array of available colors.
	UPROPERTY(SaveGame)
	int32 CurrentColor = 0;
	                     
	//Currently worn item instances.
	UPROPERTY(SaveGame)
	TArray<FBSEquipmentInstance> Equipment;
	
	//Items persistently unlocked to inventory. These are intended to be selected and equipped in the hub.
	UPROPERTY(SaveGame)
	TArray<TSubclassOf<UBSEquipment>> Inventory;
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
	UPROPERTY(SaveGame)
	FBSSaveData SaveData;
	                                                                                                           
	//Default effects any player has regardless of gear. 
	//Default attributes and other persistent effect are good to have here.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Initialization Data")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;
	
	//Just here for testing purposes. Will be deprecated when equipment is fully implemented
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Initialization Data")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
	
	//Default tags such as progression, unlocks and factions.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Initialization Data")
	TMap<FGameplayTag, int32> DefaultTags;
	
	//Default Equipment to spawn with when first starting a new game. 
	//Also, a fallback should the saved equipment be invalid or fail
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Initialization Data")
	TArray<TSubclassOf<UBSEquipment>> DefaultEquipment;
};
