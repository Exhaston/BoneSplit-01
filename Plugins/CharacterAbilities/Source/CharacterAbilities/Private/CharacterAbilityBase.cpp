// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "CharacterAbilityBase.h"

#include "AbilitiesExtensionLib.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CooldownGameplayEffect.h"
#include "DefaultAttributeSet.h"
#include "CharacterAbilities.h"
#include "GameFramework/Character.h"

UCharacterAbilityBase::UCharacterAbilityBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	bReplicateInputDirectly = true;
	
	CostGameplayEffectClass = nullptr;
	CooldownGameplayEffectClass = UCooldownGameplayEffect::StaticClass();
}

void UCharacterAbilityBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		CharacterAvatar = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	}
}

bool UCharacterAbilityBase::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, 
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!CharacterAvatar.IsValid())
	{
		const FString DebugNoCharacter = GetName() + ": No Available Character Avatar Found. Aborting Activation.";
		UE_LOG(CharacterAbilities, Warning, TEXT("%s"), *DebugNoCharacter);
		return false;
	}
	
	if (!MovementRequirementsMet() || !IsMovementModeCompatible()) return false;
	
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;
	
	if (ActivationGroup == EAG_Independent) return true;
	
	TArray<FGameplayAbilitySpec>& Specs = ActorInfo->AbilitySystemComponent->GetActivatableAbilities();
	TArray<UCharacterAbilityBase*> AbilityInstances;
		
	for (auto& AbilitySpec : Specs)
	{
		UCharacterAbilityBase* CurrentInstance = 
			Cast<UCharacterAbilityBase>(AbilitySpec.GetPrimaryInstance());
		
		if (CurrentInstance && CurrentInstance->IsActive() && CurrentInstance->ActivationGroup != EAG_Independent)
		{
			AbilityInstances.Add(CurrentInstance);
		}
	}
	
	if (AbilityInstances.FindByPredicate([this](const UCharacterAbilityBase* AbilityInstance)
	{
		return AbilityInstance->ActivationGroup == EAG_ExclusiveBlocking;
	}))
	{
		return false;
	}
	
	return true;
}

bool UCharacterAbilityBase::CommitAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayTagContainer* OptionalRelevantTags)
{
	const bool bSuccessfulCommit = Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
	
	if (bSuccessfulCommit && ActivationGroup != EAG_Independent) //Actually cancel (replace) old replaceable abilities.
	{
		FScopedAbilityListLock ActiveScopeLock(*ActorInfo->AbilitySystemComponent);
		
		TArray<FGameplayAbilitySpec>& Specs = 
			ActorInfo->AbilitySystemComponent->GetActivatableAbilities();

		for (FGameplayAbilitySpec& Spec : Specs)
		{
			if (!Spec.IsActive()) continue;
			
			UCharacterAbilityBase* CurrentInstance = 
			Cast<UCharacterAbilityBase>(Spec.GetPrimaryInstance());
			
			if (CurrentInstance && CurrentInstance->ActivationGroup == EAG_ExclusiveReplaceable)
			{
				if (CurrentInstance == this && !bCanCancelSelf) continue;
				CurrentInstance->CancelAbility(
				CurrentInstance->GetCurrentAbilitySpecHandle(), 
				CurrentInstance->GetCurrentActorInfo(), 
				CurrentInstance->GetCurrentActivationInfo(), 
				true);
			}
		}
	}

	
	return bSuccessfulCommit;
}

void UCharacterAbilityBase::CommitExecute(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!bApplyCooldownOnEnd) ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	if (!bApplyCostOnEnd) ApplyCost(Handle, ActorInfo, ActivationInfo);
}

void UCharacterAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (bApplyCooldownOnEnd) ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	if (bApplyCostOnEnd) ApplyCost(Handle, ActorInfo, ActivationInfo);
}

ACharacter* UCharacterAbilityBase::GetCharacterAvatar() const
{
	return CharacterAvatar.IsValid() ? CharacterAvatar.Get() : nullptr;
}

const FGameplayTagContainer* UCharacterAbilityBase::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	
	// MutableTags writes to the TempCooldownTags on the 
	// CDO so clear it in case the ability cooldown tags change (moved to a different slot)
	
	MutableTags->Reset(); 
	if (const FGameplayTagContainer* ParentTags = Super::GetCooldownTags())
	{
		MutableTags->AppendTags(*ParentTags);
	}
	
	MutableTags->AppendTags(CooldownTags);
	
	return MutableTags;
}

void UCharacterAbilityBase::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CooldownDuration.GetValueAtLevel(GetAbilityLevel()) <= 0) return;
	if (GetCooldownModifier()  >= 1) return; //No CD.
	if (const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		const FGameplayEffectSpecHandle SpecHandle = 
			MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			DefaultSetByCallerTags::SetByCaller_Duration, 
			CooldownDuration.GetValueAtLevel(GetAbilityLevel()) * (1 - GetCooldownModifier()));
		
		// ReSharper disable once CppExpressionWithoutSideEffects
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

bool UCharacterAbilityBase::CheckCost(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	const UAbilitySystemComponent* OwnerAsc = GetAbilitySystemComponentFromActorInfo();
	for (auto& CostPair : AbilityCost)
	{
		if (!OwnerAsc->HasAttributeSetForAttribute(CostPair.Key)) return false;
		if (OwnerAsc->GetNumericAttribute(CostPair.Key) < CostPair.Value * GetCostModifier())
		{
			const FGameplayTag& CostTag = 
				UAbilitySystemGlobals::Get().ActivateFailCostTag;

			if (OptionalRelevantTags && CostTag.IsValid())
			{
				OptionalRelevantTags->AddTag(CostTag);
			}
			return false;
		}
	}
	return true;
}

float UCharacterAbilityBase::GetCostModifier() const
{
	return 1;
}

float UCharacterAbilityBase::GetCooldownModifier() const
{
	return 0;
}

void UCharacterAbilityBase::ApplyCost(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UAbilitySystemComponent* OwnerAsc = GetAbilitySystemComponentFromActorInfo();
	for (auto& CostPair : AbilityCost)
	{
		OwnerAsc->ApplyModToAttribute(CostPair.Key, EGameplayModOp::AddBase, -CostPair.Value * GetCostModifier());
	}
}

bool UCharacterAbilityBase::IsMovementModeCompatible() const
{
	const EMovementMode CurrentMovementMode = UAbilitiesExtensionLib::GetCharacterMovementMode(GetCharacterAvatar());
	if (CurrentMovementMode == MOVE_Custom && CompatibleMovementModes.Contains(MOVE_Custom))
	{
		return CompatibleCustomMovementModes.Contains(
		UAbilitiesExtensionLib::GetCharacterCustomMovementMode(GetCharacterAvatar()));
	}
	return CompatibleMovementModes.Contains(CurrentMovementMode);
}

bool UCharacterAbilityBase::MovementRequirementsMet() const
{
	if (MaxAllowedVelocity >= 0)
	{
		return GetCharacterAvatar()->GetVelocity().Length() <= MaxAllowedVelocity;
	}
	return true;
}
