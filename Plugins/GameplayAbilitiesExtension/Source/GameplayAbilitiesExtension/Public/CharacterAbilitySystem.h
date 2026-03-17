// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CharacterAbilitySystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnDamageEvent, 
	UAbilitySystemComponent*, SourceAsc,
	UAbilitySystemComponent*, TargetAsc, 
	FGameplayTagContainer, EffectTags, 
	float, BaseDamage, 
	float, TotalDamage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealEvent, 
	UAbilitySystemComponent*, SourceAsc,
	UAbilitySystemComponent*, TargetAsc, 
	FGameplayTagContainer, EffectTags, 
	float, BaseHeal, 
	float, TotalHeal);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilityGranted, FGameplayAbilitySpec& AbilitySpec);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYABILITIESEXTENSION_API UCharacterAbilitySystem : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	
	UCharacterAbilitySystem();
	
	UPROPERTY(BlueprintAssignable)
	FOnDamageEvent OnDamageDealt;
	
	UPROPERTY(BlueprintAssignable)
	FOnHealEvent OnHealingDealt;
	
	UPROPERTY(BlueprintAssignable)
	FOnDamageEvent OnDamageTakenDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnHealEvent OnHealedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnDamageEvent OnHealthZero;
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnDamageDealt(
		UAbilitySystemComponent* SourceAsc,
		UAbilitySystemComponent* TargetAsc, 
		FGameplayTagContainer EffectTags, 
		float BaseDamage, 
		float TotalDamage,
		const FGameplayEffectSpec& EffectSpec);
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnHealthReachZero(
		UAbilitySystemComponent* SourceAsc,
		UAbilitySystemComponent* TargetAsc, 
		FGameplayTagContainer EffectTags, 
		float BaseDamage, 
		float TotalDamage,
		const FGameplayEffectSpec& EffectSpec);
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnDamageTaken(
		UAbilitySystemComponent* SourceAsc,
		UAbilitySystemComponent* TargetAsc, 
		FGameplayTagContainer EffectTags, 
		float BaseDamage, 
		float TotalDamage,
		const FGameplayEffectSpec& EffectSpec);
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnHealReceived(
		UAbilitySystemComponent* SourceAsc,
		UAbilitySystemComponent* TargetAsc, 
		FGameplayTagContainer EffectTags, 
		float BaseHeal, 
		float TotalHeal,
		const FGameplayEffectSpec& EffectSpec);
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnHealingDealt(
		UAbilitySystemComponent* SourceAsc,
		UAbilitySystemComponent* TargetAsc, 
		FGameplayTagContainer EffectTags, 
		float BaseHeal, 
		float TotalHeal,
		const FGameplayEffectSpec& EffectSpec);
	
	static void AdjustBlendTimeForMontage(
	const UAnimInstance* TargetAnimInstance, const UAnimMontage* Montage, float InRate);

	virtual float PlayMontage(
		UGameplayAbility* AnimatingAbility, 
		FGameplayAbilityActivationInfo ActivationInfo, 
		UAnimMontage* Montage, 
		float InPlayRate, 
		FName StartSectionName = NAME_None, 
		float StartTimeSeconds = 0.0f) override;
	
	virtual float PlayMontageSimulated(
		UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None) override;
	
	virtual bool CancelAbilitiesWithTag(FGameplayTag InTag);
	
	FOnAbilityGranted OnAbilityGrantedDelegate;
	
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilities;
	
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	void NotifyAbilitiesTo(FOnAbilityGranted::FDelegate&& Callback);
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
};
