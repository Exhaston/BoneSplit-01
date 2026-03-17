// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "CharacterAbilitySystem.h"


UCharacterAbilitySystem::UCharacterAbilitySystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterAbilitySystem::NetMulticast_OnDamageDealt_Implementation(UAbilitySystemComponent* SourceAsc,
	UAbilitySystemComponent* TargetAsc, FGameplayTagContainer EffectTags, float BaseDamage, float TotalDamage,
	const FGameplayEffectSpec& EffectSpec)
{
	OnDamageDealt.Broadcast(SourceAsc, TargetAsc, EffectTags, BaseDamage, TotalDamage);
}

void UCharacterAbilitySystem::NetMulticast_OnDamageTaken_Implementation(UAbilitySystemComponent* SourceAsc,
	UAbilitySystemComponent* TargetAsc, FGameplayTagContainer EffectTags, float BaseDamage, float TotalDamage,
	const FGameplayEffectSpec& EffectSpec)
{
	OnDamageTakenDelegate.Broadcast(SourceAsc, TargetAsc, EffectTags, BaseDamage, TotalDamage);
}

void UCharacterAbilitySystem::NetMulticast_OnHealReceived_Implementation(UAbilitySystemComponent* SourceAsc,
	UAbilitySystemComponent* TargetAsc, FGameplayTagContainer EffectTags, float BaseHeal, float TotalHeal,
	const FGameplayEffectSpec& EffectSpec)
{
	OnHealedDelegate.Broadcast(SourceAsc, TargetAsc, EffectTags, BaseHeal, TotalHeal);
}

void UCharacterAbilitySystem::NetMulticast_OnHealingDealt_Implementation(UAbilitySystemComponent* SourceAsc,
	UAbilitySystemComponent* TargetAsc, FGameplayTagContainer EffectTags, float BaseHeal, float TotalHeal,
	const FGameplayEffectSpec& EffectSpec)
{
	OnHealingDealt.Broadcast(SourceAsc, TargetAsc, EffectTags, BaseHeal, TotalHeal);
}

void UCharacterAbilitySystem::NetMulticast_OnHealthReachZero_Implementation(UAbilitySystemComponent* SourceAsc,
	UAbilitySystemComponent* TargetAsc, FGameplayTagContainer EffectTags, float BaseDamage, float TotalDamage,
	const FGameplayEffectSpec& EffectSpec)
{
	OnHealthZero.Broadcast(SourceAsc, TargetAsc, EffectTags, BaseDamage, TotalDamage);
}

void UCharacterAbilitySystem::AdjustBlendTimeForMontage(
	const UAnimInstance* TargetAnimInstance, const UAnimMontage* Montage, const float InRate)
{
	if (!TargetAnimInstance || !Montage || FMath::IsNearlyZero(InRate)) return;
	FAnimMontageInstance* MontageInstance = TargetAnimInstance->GetInstanceForMontage(Montage);
	MontageInstance->DefaultBlendTimeMultiplier = 1 / InRate;
}

float UCharacterAbilitySystem::PlayMontage(
	UGameplayAbility* AnimatingAbility,
	const FGameplayAbilityActivationInfo ActivationInfo, 
	UAnimMontage* Montage,
	const float InPlayRate,
	const FName StartSectionName,
	const float StartTimeSeconds)
{
	if (!Montage)
	{
		FString DebugNoMontage = AnimatingAbility->GetName() + ": Executed with null montage";
		return 0;
	}
	
	const float AnimationLength = Super::PlayMontage(
		AnimatingAbility, ActivationInfo, Montage, InPlayRate, StartSectionName, StartTimeSeconds);
	
	AdjustBlendTimeForMontage(AbilityActorInfo->GetAnimInstance(), Montage, InPlayRate);
	return AnimationLength;
}

float UCharacterAbilitySystem::PlayMontageSimulated(
	UAnimMontage* Montage, const float InPlayRate, const FName StartSectionName)
{
	const float AnimationLength = Super::PlayMontageSimulated(Montage, InPlayRate, StartSectionName);
	
	AdjustBlendTimeForMontage(AbilityActorInfo->GetAnimInstance(), Montage, InPlayRate);
	return AnimationLength;
}

bool UCharacterAbilitySystem::CancelAbilitiesWithTag(const FGameplayTag InTag)
{
	bool Result = false;
	
	TArray<FGameplayAbilitySpec*> FoundSpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(
		FGameplayTagContainer(InTag), FoundSpecs);

	for (const auto FoundSpec : FoundSpecs)
	{
		if (FoundSpec->GetPrimaryInstance()->IsActive())
		{
			Result = true;
			CancelAbilityHandle(FoundSpec->Handle);
		}
	}
	
	return Result;
}

void UCharacterAbilitySystem::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	GrantedAbilities.Add(AbilitySpec.Handle);
	OnAbilityGrantedDelegate.Broadcast(AbilitySpec);
}

void UCharacterAbilitySystem::NotifyAbilitiesTo(FOnAbilityGranted::FDelegate&& Callback)
{
	for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilities)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle))
		{
			Callback.Execute(*Spec);
		}
	}
	OnAbilityGrantedDelegate.Add(MoveTemp(Callback));
}

void UCharacterAbilitySystem::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);
	GrantedAbilities.Remove(AbilitySpec.Handle);
}