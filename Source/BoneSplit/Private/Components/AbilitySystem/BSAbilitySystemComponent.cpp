// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/BSAbilitySystemComponent.h"

#include "Actors/Predictables/BSPredictableActor.h"


UBSAbilitySystemComponent::UBSAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBSAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);
	if (GetCurrentMontage())
	{
		StopMontageIfCurrent(*GetCurrentMontage(), -1);
	}
}

void UBSAbilitySystemComponent::AdjustBlendTimeForMontage(
	const UAnimInstance* TargetAnimInstance, const UAnimMontage* Montage, const float InRate)
{
	check(TargetAnimInstance);
	check(Montage);
	if (FMath::IsNearlyZero(InRate)) return;
	FAnimMontageInstance* MontageInstance = TargetAnimInstance->GetInstanceForMontage(Montage);
	MontageInstance->DefaultBlendTimeMultiplier = 1 / InRate;
}

float UBSAbilitySystemComponent::PlayMontage(
	UGameplayAbility* AnimatingAbility,
	const FGameplayAbilityActivationInfo ActivationInfo, 
	UAnimMontage* Montage,
	const float InPlayRate,
	const FName StartSectionName,
	const float StartTimeSeconds)
{
	check(Montage);
	
	const float AnimationLength = Super::PlayMontage(
		AnimatingAbility, ActivationInfo, Montage, InPlayRate, StartSectionName, StartTimeSeconds);
	
	AdjustBlendTimeForMontage(AbilityActorInfo->GetAnimInstance(), Montage, InPlayRate);
	return AnimationLength;
}

float UBSAbilitySystemComponent::PlayMontageSimulated(
	UAnimMontage* Montage, const float InPlayRate, const FName StartSectionName)
{
	const float AnimationLength = Super::PlayMontageSimulated(Montage, InPlayRate, StartSectionName);
	
	AdjustBlendTimeForMontage(AbilityActorInfo->GetAnimInstance(), Montage, InPlayRate);
	return AnimationLength;
}

bool UBSAbilitySystemComponent::CancelAbilitiesWithTag(const FGameplayTag InTag)
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

FGameplayEffectContextHandle UBSAbilitySystemComponent::MakeEffectContext() const
{
	FGameplayEffectContextHandle Handle = Super::MakeEffectContext();
	if (!Handle.HasOrigin())
	{
		Handle.AddOrigin(GetAvatarActor()->GetActorLocation());
	}
	return Handle;
}

