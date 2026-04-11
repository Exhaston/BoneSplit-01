// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSDamageEffect.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BSGameplayAbilitiesLibrary.generated.h"

enum class ECAAbilitySystemGetResult : uint8;
class UBSBuffEffect;
class UBSHealingEffect;
class UAbilitySystemComponent;
class UCharacterAbilitySystem;
class UBSCharacterInitData;

USTRUCT(BlueprintType, Blueprintable, DisplayName="AdditionalEffects")
struct FBSAdditionalEffectInfo
{
	GENERATED_BODY()
	
	FBSAdditionalEffectInfo() = default;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAppendDynamicTagsFromDamage = false;
	
	//If 0 will use the original Duration of the spec
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<TSubclassOf<UBSBuffEffect>, float> BuffDurationMap;
};
									 
UENUM(BlueprintType, Blueprintable, DisplayName="Effect Application Result")
enum class EBSEffectApplicationResult : uint8
{
	Success = 0 UMETA(DisplayName = "Success"),
	Failed = 1 UMETA(DisplayName = "Failed"),
};

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSGameplayAbilitiesLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	template<typename KeyType>
	static KeyType GetRandomArrayElem(const TArray<KeyType>& InArray)
	{
		if (InArray.IsEmpty()) return nullptr;
		return InArray[FMath::RandRange(0, InArray.Num() - 1)];
	}
	
	
	UFUNCTION(BlueprintCallable, Category="Trap|Utility")
	static FVector ProjectToGround(UObject* ContextObject, FVector InLocation);
	
	UFUNCTION(BlueprintCallable)
	static FVector GetTargetLocationFromActor(AActor* Actor);
	
	static void ApplyCharacterEffects(UBSCharacterInitData* CharacterInitData, UCharacterAbilitySystem* CharacterAbilitySystem, int32 InLevel = 0);
	static void ApplyCharacterAbilities(UBSCharacterInitData* CharacterInitData, UCharacterAbilitySystem* CharacterAbilitySystem, int32 InLevel = 0);
	static void ApplyCharacterAttributes(UBSCharacterInitData* CharacterInitData, UCharacterAbilitySystem* CharacterAbilitySystem, int32 InLevel = 0);
	static void ApplyCharacterDataTo(
		UBSCharacterInitData* CharacterInitData, UCharacterAbilitySystem* CharacterAbilitySystem, int32 InLevel = 0);
	
	UFUNCTION(BlueprintCallable)
	static FRotator GetLookToTargetRotation(APawn* MobPawn, bool bOnlyPitch = true);
	
	static bool PassFactionCheck(AActor* InActor, AActor* InTarget, bool bHitFriendly, bool bHitEnemy, bool bHitSelf);
	
	//Supports instant effects by default, but accepts children of the damage effect class. 
	//These can be modified further.
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "OutResult"))
	static void ApplyDamageTo(
		UAbilitySystemComponent* Instigator, 
		UAbilitySystemComponent* Target,
		FBSAdditionalEffectInfo AdditionalEffects,
		EBSEffectApplicationResult& OutResult,
		bool CustomOrigin = false,
		FVector Origin = FVector::ZeroVector,
		bool bHitSelf = false,
		bool bHitAllies = false,
		bool bHitEnemies = true,
		TSubclassOf<UBSDamageEffect> DamageEffectClass = nullptr,
		float Damage = 10, 
		float Level = 1, 
		float Knockback = 0, 
		FGameplayTagContainer EffectTags = FGameplayTagContainer());
	
	//Supports instant effects by default, but accepts children of the damage effect class. 
	//These can be modified further.
	UFUNCTION(BlueprintCallable)
	static bool ApplyHealingTo(
		UAbilitySystemComponent* Instigator, 
		UAbilitySystemComponent* Target,
		bool CustomOrigin = false,
		FVector Origin = FVector::ZeroVector,
		bool bHitSelf = true,
		bool bHitAllies = true,
		bool bHitEnemies = false,
		TSubclassOf<UBSHealingEffect> HealingEffectClass = nullptr,
		float Healing = 10, 
		float Level = 1, 
		float Knockback = 0, 
		FGameplayTagContainer EffectTags = FGameplayTagContainer());
	
	//Supports instant effects by default, but accepts children of the damage effect class. 
	//These can be modified further.
	UFUNCTION(BlueprintCallable)
	static bool ApplyBuffTo(
		UAbilitySystemComponent* Instigator, 
		UAbilitySystemComponent* Target,
		bool CustomOrigin = false,
		FVector Origin = FVector::ZeroVector,
		bool bHitSelf = true,
		bool bHitAllies = true,
		bool bHitEnemies = false,
		TSubclassOf<UBSBuffEffect> BuffEffectClass = nullptr,
		float Duration = 10, 
		float Level = 1, 
		FGameplayTagContainer EffectTags = FGameplayTagContainer());
};
