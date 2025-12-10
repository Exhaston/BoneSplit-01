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

USTRUCT(Blueprintable, BlueprintType)
struct FBSSaveData
{
	GENERATED_BODY()
	
	UPROPERTY(SaveGame)
	TArray<FBSAttributeData> Attributes;
	
	UPROPERTY(SaveGame)
	TArray<FBSGameplayEffectData> Effects;
	
	UPROPERTY(SaveGame)
	TArray<FGameplayTag> Tags;
	
	UPROPERTY(SaveGame)
	int32 CurrentColor = 0;
	                     
	//Current worn item instances, will be reset when visiting the hub
	UPROPERTY(SaveGame)
	TArray<FBSEquipmentInstance> Equipment;
	
	//Available items for spawning into a run with
	UPROPERTY(SaveGame)
	TArray<TSubclassOf<UBSEquipment>> Inventory;
};

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(SaveGame)
	FBSSaveData SaveData;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Initialization Data")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Initialization Data")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Initialization Data")
	TArray<FGameplayTag> DefaultTags;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Initialization Data")
	TArray<TSubclassOf<UBSEquipment>> DefaultEquipment;
};
