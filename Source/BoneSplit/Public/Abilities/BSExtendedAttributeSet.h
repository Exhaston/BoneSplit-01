// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefaultAttributeSet.h"
#include "BSExtendedAttributeSet.generated.h"

namespace BSExtendedAttributeTags
{
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_SuccessfulBlock);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_God);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_HealthLeeching);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_ManaLeeching);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_SoulCharging);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Blockable);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Knockback);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_PVPFlagged);
}

UCLASS()
class BONESPLIT_API UBSExtendedAttributeSet : public UDefaultAttributeSet
{
	GENERATED_BODY()
	
public:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	virtual float GetMaxForAttribute(const FGameplayAttribute& Attribute) const override;
	virtual float GetMinForAttribute(const FGameplayAttribute& Attribute) const override;
	
	virtual void HandleKnockback(UAbilitySystemComponent* TargetAsc, float& KnockbackToApply, const FVector Origin);
	
	virtual void HandleHealing(UCharacterAbilitySystem* InstigatorAsc, UCharacterAbilitySystem* TargetAsc, float BaseHeal, float& HealToApply, FGameplayTagContainer EffectTags, const FGameplayEffectSpec& EffectSpec) override;
	virtual void HandleDamage(UCharacterAbilitySystem* InstigatorAsc, UCharacterAbilitySystem* TargetAsc, float BaseDamage, float& DamageToApply, FGameplayTagContainer EffectTags, const FGameplayEffectSpec& EffectSpec) override;
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Threat = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, Threat)
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Knockback = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, Knockback)
	
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PowerModifier)
	FGameplayAttributeData Power = 0; //0 = only base damage on effects.
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, Power)

	UFUNCTION()
	virtual void OnRep_PowerModifier(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, Power, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalChance)
	FGameplayAttributeData CriticalChance = 0; //Default to 0 for mobs.
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, CriticalChance)

	UFUNCTION()
	virtual void OnRep_CriticalChance(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, CriticalChance, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalModifier)
	FGameplayAttributeData CriticalModifier = 1; //1 = Normal Damage multiplier
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, CriticalModifier)

	UFUNCTION()
	virtual void OnRep_CriticalModifier(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, CriticalModifier, OldAttributeData);
	}
	
	// =================================================================================================================
	// Mana
	// ================================================================================================================= 
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentMana)
	FGameplayAttributeData CurrentMana = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, CurrentMana)
	
	UFUNCTION()
	virtual void OnRep_CurrentMana(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, CurrentMana, OldAttributeData);
	}

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, MaxMana)
	
	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, MaxMana, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AbilityCostModifier)
	FGameplayAttributeData AbilityCostModifier = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, AbilityCostModifier)
	
	UFUNCTION()
	virtual void OnRep_AbilityCostModifier(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, AbilityCostModifier, OldAttributeData);
	}
	
	// =================================================================================================================
	// Secondary
	// ================================================================================================================= 
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RangeModifier)
	FGameplayAttributeData RangeModifier = 1;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, RangeModifier)
	
	UFUNCTION()
	virtual void OnRep_RangeModifier(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, RangeModifier, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MultiHit)
	FGameplayAttributeData MultiHit = 1;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, MultiHit)
	
	UFUNCTION()
	virtual void OnRep_MultiHit(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, MultiHit, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Thorns)
	FGameplayAttributeData Thorns = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, Thorns)
	
	UFUNCTION()
	virtual void OnRep_Thorns(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, Thorns, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Leech)
	FGameplayAttributeData Leech = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, Leech)
	
	UFUNCTION()
	virtual void OnRep_Leech(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, Leech, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaLeech)
	FGameplayAttributeData ManaLeech = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, ManaLeech)
	
	UFUNCTION()
	virtual void OnRep_ManaLeech(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, ManaLeech, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance)
	FGameplayAttributeData BlockChance = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, BlockChance)
	
	UFUNCTION()
	virtual void OnRep_BlockChance(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, BlockChance, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, Armor)
	
	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, Armor, OldAttributeData);
	}
	
	// =================================================================================================================
	// Speed - Modifier to movement
	// ================================================================================================================= 

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Speed)
	FGameplayAttributeData Speed = 1;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, Speed)
	
	UFUNCTION()
	virtual void OnRep_Speed(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, Speed, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Friction)
	FGameplayAttributeData Friction = 1;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, Friction)
	
	UFUNCTION()
	virtual void OnRep_Friction(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, Friction, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_JumpHeight)
	FGameplayAttributeData JumpHeight = 1.5;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, JumpHeight)
	
	UFUNCTION()
	virtual void OnRep_JumpHeight(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, JumpHeight, OldAttributeData);
	}	
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AirControl)
	FGameplayAttributeData AirControl = 0.1;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, AirControl)
	
	UFUNCTION()
	virtual void OnRep_AirControl(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, AirControl, OldAttributeData);
	}
	
	// =================================================================================================================
	// Soul Charge - When charged to 100 soul ability is available (Ult gauge)
	// =================================================================================================================

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SoulCharge)
	FGameplayAttributeData SoulCharge = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, SoulCharge)
	
	UFUNCTION()
	virtual void OnRep_SoulCharge(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, SoulCharge, OldAttributeData);
	}	
	
	//Non replicated "static" value for simplicity when selecting attributes
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxSoulCharge = 1;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, MaxSoulCharge)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SoulChargeAmount)
	FGameplayAttributeData SoulChargeAmount = 0.01;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, SoulChargeAmount)
	
	UFUNCTION()
	virtual void OnRep_SoulChargeAmount(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, SoulChargeAmount, OldAttributeData);
	}	
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CooldownReductionModifier)
	FGameplayAttributeData CooldownReductionModifier = 0;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, CooldownReductionModifier)
	
	UFUNCTION()
	virtual void OnRep_CooldownReductionModifier(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, CooldownReductionModifier, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed = 1;
	ATTRIBUTE_ACCESSORS(UBSExtendedAttributeSet, AttackSpeed)
	
	UFUNCTION()
	virtual void OnRep_AttackSpeed(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSExtendedAttributeSet, AttackSpeed, OldAttributeData);
	}
};
