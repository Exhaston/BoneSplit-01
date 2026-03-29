// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "NativeGameplayTags.h"
#include "DefaultAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class UCharacterAbilitySystem;

namespace DefaultSetByCallerTags
{
	CHARACTERABILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Shield);
	CHARACTERABILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_CurrentHealth);
	CHARACTERABILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_MaxHealth);
	CHARACTERABILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	CHARACTERABILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Healing);
	CHARACTERABILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Duration);
}

/**
 * 
 */
UCLASS(DisplayName="Default Attribute Set")
class CHARACTERABILITIES_API UDefaultAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	virtual float GetMaxForAttribute(const FGameplayAttribute& Attribute) const;
	virtual float GetMinForAttribute(const FGameplayAttribute& Attribute) const;
	
	static void AdjustAttributeForMaxChange(
		UAbilitySystemComponent* OwningAsc,
		const FGameplayAttribute& AffectedAttribute, 
		const float OldValue, 
		const float NewMaxValue);
	
	// =================================================================================================================
	// Health
	// ================================================================================================================= 
	
	UPROPERTY()
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, Damage)
	
	UPROPERTY()
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, Healing)
	
	UPROPERTY(ReplicatedUsing = OnRep_Shield)
	FGameplayAttributeData Shield = 0;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, Shield)

	UFUNCTION()
	virtual void OnRep_Shield(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, Shield, OldAttributeData);
	}
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	FGameplayAttributeData CurrentHealth = 0;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, CurrentHealth)

	UFUNCTION()
	virtual void OnRep_CurrentHealth(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, CurrentHealth, OldAttributeData);
	}

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, DisplayName="Max Health")
	FGameplayAttributeData MaxHealth = 0;
	ATTRIBUTE_ACCESSORS(UDefaultAttributeSet, MaxHealth)

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldAttributeData)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UDefaultAttributeSet, MaxHealth, OldAttributeData);
	}
	
protected:
	
	virtual void HandleHealing(
		UCharacterAbilitySystem* InstigatorAsc, 
		UCharacterAbilitySystem* TargetAsc, 
		float BaseHeal, 
		float& HealToApply, 
		FGameplayTagContainer EffectTags, 
		const FGameplayEffectSpec& EffectSpec);
	
	virtual void HandleDamage(
		UCharacterAbilitySystem* InstigatorAsc, 
		UCharacterAbilitySystem* TargetAsc, 
		float BaseDamage, 
		float& DamageToApply, 
		FGameplayTagContainer EffectTags, 
		const FGameplayEffectSpec& EffectSpec);
};
