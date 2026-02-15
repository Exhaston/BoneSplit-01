// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "BSAttributeSet.generated.h"

#pragma region Macros

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#pragma endregion

UCLASS()
class BONESPLIT_API UBSAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
#pragma region AttributeEvents
	
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	float GetMaxForAttribute(const FGameplayAttribute& Attribute) const;

	static float GetMinForAttribute(const FGameplayAttribute& Attribute);
	
	void AdjustAttributeForMaxChange(
		const FGameplayAttribute& AffectedAttribute, 
		float OldValue,
		float NewMaxValue) const;
	
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
#pragma endregion

#pragma region MetaAttributes
	
	// =================================================================================================================
	// META
	// ================================================================================================================= 
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData PhysicalDamage;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, PhysicalDamage)
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MagicDamage;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, MagicDamage)
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData TrueDamage;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, TrueDamage)
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Healing)
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Threat;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Threat)
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Knockback;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Knockback)
	
#pragma endregion
	
#pragma region Attributes
	
	// =================================================================================================================
	// Health
	// ================================================================================================================= 
	
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health = 100;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Health)

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, Health, OldAttributeData);
	}
	
	// =================================================================================================================
	// MaxHealth Default -1 to avoid clamping before set properly.
	// ================================================================================================================= 

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, DisplayName="Max Health")
	FGameplayAttributeData MaxHealth = -1;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, MaxHealth)

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, MaxHealth, OldAttributeData);
	}
	
	// =================================================================================================================
	// Shield
	// ================================================================================================================= 
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Shield)
	FGameplayAttributeData Shield = 0;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Shield)

	UFUNCTION()
	virtual void OnRep_Shield(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, Shield, OldAttributeData);
	}
	                  
	// =================================================================================================================
	// Mana
	// ================================================================================================================= 
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana = 100;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Mana)
	
	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, Mana, OldAttributeData);
	}
	
	// =================================================================================================================
	// MaxMana Default -1 to avoid clamping before set properly.
	// ================================================================================================================= 

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana = -1;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, MaxMana)
	
	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, MaxMana, OldAttributeData);
	}

	// =================================================================================================================
	// Power
	// ================================================================================================================= 
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Power)
	FGameplayAttributeData Power = 10;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Power)
	
	UFUNCTION()
	virtual void OnRep_Power(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, Power, OldAttributeData);
	}

	// =================================================================================================================
	// AttackSpeed
	// ================================================================================================================= 

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed = 1;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, AttackSpeed)
	
	UFUNCTION()
	virtual void OnRep_AttackSpeed(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, AttackSpeed, OldAttributeData);
	}

	// =================================================================================================================
	// CritChance - Every effect has a roll against this to increase the power of that effect.
	// ================================================================================================================= 

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritChance)
	FGameplayAttributeData CritChance = 5;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, CritChance)
	
	UFUNCTION()
	virtual void OnRep_CritChance(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, CritChance, OldAttributeData);
	}
	
	// =================================================================================================================
	// CritMod - Multiplier to the effect if the attack rolled a crit
	// ================================================================================================================= 

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritMod)
	FGameplayAttributeData CritMod = 1.25;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, CritMod)
	
	UFUNCTION()
	virtual void OnRep_CritMod(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, CritMod, OldAttributeData);
	}

	// =================================================================================================================
	// Cooldown Reduction - Reduces the cooldown time of abilities and procs
	// ================================================================================================================= 

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CooldownReduction)
	FGameplayAttributeData CooldownReduction = 0;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, CooldownReduction)
	
	UFUNCTION()
	virtual void OnRep_CooldownReduction(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, CooldownReduction, OldAttributeData);
	}

	// =================================================================================================================
	// Speed - Modifier to movement
	// ================================================================================================================= 

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Speed)
	FGameplayAttributeData Speed = 1;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Speed)
	
	UFUNCTION()
	virtual void OnRep_Speed(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, Speed, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Friction)
	FGameplayAttributeData Friction = 1;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Friction)
	
	UFUNCTION()
	virtual void OnRep_Friction(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, Friction, OldAttributeData);
	}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_JumpHeight)
	FGameplayAttributeData JumpHeight = 1.5;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, JumpHeight)
	
	UFUNCTION()
	virtual void OnRep_JumpHeight(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, JumpHeight, OldAttributeData);
	}	
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AirControl)
	FGameplayAttributeData AirControl = 0.1;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, AirControl)
	
	UFUNCTION()
	virtual void OnRep_AirControl(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, AirControl, OldAttributeData);
	}

	// =================================================================================================================
	// Reach - Distance and range of attacks
	// ================================================================================================================= 
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Reach)
	FGameplayAttributeData Reach = 1;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Reach)
	
	UFUNCTION()
	virtual void OnRep_Reach(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, Reach, OldAttributeData);
	}

	// =================================================================================================================
	// Soul Charge - When charged to 100 soul ability is available (Ult gauge)
	// =================================================================================================================

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SoulCharge)
	FGameplayAttributeData SoulCharge = 0;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, SoulCharge)
	
	UFUNCTION()
	virtual void OnRep_SoulCharge(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, SoulCharge, OldAttributeData);
	}	
	
	//Non replicated "static" value for simplicity when selecting attributes
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxSoulCharge = 1;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, MaxSoulCharge)
	
	// =================================================================================================================
	// Thorns - Reflect damage back to attackers
	// =================================================================================================================
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Thorns)
	FGameplayAttributeData Thorns = 0;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Thorns)

	UFUNCTION()
	virtual void OnRep_Thorns(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, Thorns, OldAttributeData);
	}
	
	// =================================================================================================================
	// Armor - Reduces Physical damage taken
	// =================================================================================================================

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor = 1;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Armor)

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, Armor, OldAttributeData);
	}
	
	// =================================================================================================================
	// Magic Resistance - Reduces Magic Damage taken
	// =================================================================================================================

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicRes)
	FGameplayAttributeData MagicRes = 1;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, MagicRes)
	
	UFUNCTION()
	virtual void OnRep_MagicRes(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, MagicRes, OldAttributeData);
	}
	
	// =================================================================================================================
	// Soul Gems - Currency
	// =================================================================================================================
	
	UPROPERTY(ReplicatedUsing = OnRep_SoulGems, BlueprintReadOnly, EditAnywhere)
	FGameplayAttributeData SoulGems;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, SoulGems)
	
	UFUNCTION()
	virtual void OnRep_SoulGems(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, SoulGems, OldAttributeData);
	}
	
	// =================================================================================================================
	// Gold - Currency
	// =================================================================================================================
	
	UPROPERTY(ReplicatedUsing = OnRep_Gold, BlueprintReadOnly, EditAnywhere)
	FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Gold)
	
	UFUNCTION()
	virtual void OnRep_Gold(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, Gold, OldAttributeData);
	}
	
	// =================================================================================================================
	// Bones - Currency
	// =================================================================================================================
	
	UPROPERTY(ReplicatedUsing = OnRep_Bones, Category="Currency", BlueprintReadOnly, EditAnywhere)
	FGameplayAttributeData Bones;
	ATTRIBUTE_ACCESSORS(UBSAttributeSet, Bones)
	
	UFUNCTION()
	virtual void OnRep_Bones(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UBSAttributeSet, Bones, OldAttributeData);
	}
	
#pragma endregion
};
