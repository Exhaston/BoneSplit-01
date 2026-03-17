// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitiesExtensionLib.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct GAMEPLAYABILITIESEXTENSION_API FFactionTargetMode
{
	GENERATED_BODY()
	
	FFactionTargetMode() = default;
	FFactionTargetMode(bool InSelf, bool InEnemies, bool InAllies);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Faction")
	bool bSelf = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Faction")
	bool bEnemies = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Faction")
	bool bAllies = false;
	
	bool EvaluateFaction(UAbilitySystemComponent* InComponent, UAbilitySystemComponent* InTarget);
	bool EvaluateFactionPvp(UAbilitySystemComponent* InComponent, UAbilitySystemComponent* InTarget);
};

namespace DefaultFactionTags
{
	GAMEPLAYABILITIESEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Team);
	GAMEPLAYABILITIESEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Team_Generic1);
	GAMEPLAYABILITIESEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Team_Generic2);
	
	GAMEPLAYABILITIESEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faction);
	GAMEPLAYABILITIESEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faction_Player);
	GAMEPLAYABILITIESEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faction_GenericEnemy);
	GAMEPLAYABILITIESEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faction_GenericFriendly);
	

}

namespace DefaultAbilityTags
{
	GAMEPLAYABILITIESEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability);
	GAMEPLAYABILITIESEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability);
}

UCLASS(BlueprintType)
class GAMEPLAYABILITIESEXTENSION_API UAbilitiesExtensionLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	//Reduces the efficiency of the number when it grows higher and towards the cap. 
	//Higher cap will retain higher efficiency.
	UFUNCTION(BlueprintCallable, DisplayName="Asymptotic Diminishing Returns", Category = "AbilitiesExtension|Math",
		meta=(KeyWords = "Math, BoneSplit, Scaling"))
	static float AsymptoticDr(float InValue, float InCap);
	
	//Returns true if the Ability System Components have any matching sub tags under the parent tag. 
	//Works well for faction checking, state matching etc. 
	UFUNCTION(BlueprintCallable, DisplayName="Has Matching Faction", Category = "AbilitiesExntesion|Faction",
		meta=(Keywords = "Faction, Check, Match"))
	static bool HasAnyMatchingSubTag(
		UAbilitySystemComponent* FirstAsc, UAbilitySystemComponent* SecondAsc, FGameplayTag ParentTag);
	
	//Returns true if the velocity isn't zero.
	UFUNCTION(BlueprintCallable, DisplayName="Is Moving", Category= "AbilitiesExtension|Movement", 
		meta=(Keywords = "Moving, Character"))
	static bool IsActorMoving(AActor* InActor);
	                  
	//Returns true if the pawn has a movement base it's standing on.
	UFUNCTION(BlueprintCallable, DisplayName="Is Character Grounded", Category = "AbilitiesExtension|Movement", 
		meta=(Keywords = "Grounded, Ground, Character"))
	static bool IsCharacterGrounded(ACharacter* InCharacter);
	                                                                                           
	UFUNCTION(BlueprintCallable, DisplayName="Get Character Movement Mode", Category = "AbilitiesExtension|Movement", 
	meta=(Keywords = "Grounded, Ground, MovementMode, Character"))
	static EMovementMode GetCharacterMovementMode(ACharacter* InCharacter);
	
	UFUNCTION(BlueprintCallable, DisplayName="Get Character Custom Movement Mode", Category = "AbilitiesExtension|Movement", 
	meta=(Keywords = "Grounded, Ground, MovementMode, Character"))
	static int32 GetCharacterCustomMovementMode(ACharacter* InCharacter);
	
	//Returns a key from a TMap with the format {Key, float(Weight)}. 
	//Picks random but has a higher chance of picking higher weights.
	template<typename KeyType>
	KeyType GetKeyFromWeightMap(const TMap<KeyType, float>& InWeightedMap)
	{
		float TotalWeight = 0.0f;
		for (const TPair<KeyType, float>& Pair : InWeightedMap)
		{
			TotalWeight += FMath::Max(0.0f, Pair.Value);
		}

		if (TotalWeight <= 0.0f)
		{
			return InWeightedMap.begin()->Key;
		}
		
		float Roll = FMath::FRandRange(0.0f, TotalWeight);
		
		for (const TPair<KeyType, float>& Pair : InWeightedMap)
		{
			Roll -= FMath::Max(0.0f, Pair.Value);
			if (Roll <= 0.0f)
			{
				return Pair.Key;
			}
		}
		
		return InWeightedMap.begin()->Key;
	}
};
                                                                                                      