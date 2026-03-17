// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitiesExtensionLib.h"
#include "BSDamageEffect.h"
#include "GameplayTagContainer.h"
#include "Actors/Projectiles/BSProjectile.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BSGameplayLibrary.generated.h"

class UBSBuffEffect;
class UBSHealingEffect;
struct FFactionTargetMode;
class UAbilitySystemComponent;
class UCharacterAbilitySystem;
class UBSCharacterInitData;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSGameplayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	static void ApplyCharacterDataTo(
		UBSCharacterInitData* CharacterInitData, UCharacterAbilitySystem* CharacterAbilitySystem, int32 InLevel = 0);
	
	UFUNCTION(BlueprintCallable)
	static FRotator GetLookToTargetRotation(APawn* MobPawn, bool bOnlyPitch = true);
	
	//Supports instant effects by default, but accepts children of the damage effect class. 
	//These can be modified further.
	UFUNCTION(BlueprintCallable)
	static bool ApplyDamageTo(
		UAbilitySystemComponent* Instigator, 
		UAbilitySystemComponent* Target,
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
