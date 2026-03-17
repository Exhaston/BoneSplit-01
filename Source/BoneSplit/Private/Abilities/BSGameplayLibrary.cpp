// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Abilities/BSGameplayLibrary.h"

#include "AIController.h"
#include "Components/AbilitySystem/AbilityBases/BSAbilityBase.h"
#include "CharacterAbilitySystem.h"
#include "Abilities/BSBuffEffect.h"
#include "Abilities/BSCharacterInitData.h"
#include "Abilities/BSDamageEffect.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "Abilities/BSHealingEffect.h"

void UBSGameplayLibrary::ApplyCharacterDataTo(UBSCharacterInitData* CharacterInitData,
                                              UCharacterAbilitySystem* CharacterAbilitySystem, const int32 InLevel)
{
	if (!CharacterInitData || !CharacterAbilitySystem->IsOwnerActorAuthoritative()) return;
	
	for (auto& DefaultAbilityClass : CharacterInitData->GrantedDefaultAbilities)
	{
		FGameplayAbilitySpec NewSpec = FGameplayAbilitySpec(DefaultAbilityClass);
		NewSpec.Level = InLevel;
		CharacterAbilitySystem->GiveAbility(NewSpec);
	}
	
	for (const auto& DefaultEffectClass : CharacterInitData->GrantedEffects)
	{
		FGameplayEffectSpecHandle DefaultEffectHandle = 
			CharacterAbilitySystem->MakeOutgoingSpec(
				DefaultEffectClass, 
				InLevel, 
				CharacterAbilitySystem->MakeEffectContext());
				
		CharacterAbilitySystem->ApplyGameplayEffectSpecToSelf(*DefaultEffectHandle.Data);
	}
	
	for (auto& AttributeInfoPair : CharacterInitData->AttributeMap)
	{
		CharacterAbilitySystem->ApplyModToAttribute(
			AttributeInfoPair.Key, 
			EGameplayModOp::Override, 
			AttributeInfoPair.Value.GetValueAtLevel(InLevel));
	}
	
	CharacterAbilitySystem->AddLooseGameplayTags(CharacterInitData->FactionTags);
}

FRotator UBSGameplayLibrary::GetLookToTargetRotation(APawn* MobPawn, const bool bOnlyPitch)
{
	const AAIController* OwnerController = MobPawn->GetController<AAIController>();
	if (!OwnerController) return FRotator::ZeroRotator;
	const AActor* TargetActor = OwnerController->GetFocusActorForPriority(EAIFocusPriority::Gameplay);
	if (!TargetActor) return FRotator::ZeroRotator;

	const FVector Dir = (TargetActor->GetActorLocation() - MobPawn->GetActorLocation()).GetSafeNormal();
	
	FRotator FinalRot = Dir.Rotation();
	
	if (bOnlyPitch)
	{
		FinalRot.Yaw = MobPawn->GetActorRotation().Yaw;
	}
	return FinalRot;
}

bool UBSGameplayLibrary::ApplyDamageTo(
	UAbilitySystemComponent* Instigator, 
	UAbilitySystemComponent* Target,
	const bool CustomOrigin,
	const FVector Origin,
	const bool bHitSelf,
	const bool bHitAllies,
	const bool bHitEnemies,
	TSubclassOf<UBSDamageEffect> DamageEffectClass,
	const float Damage, 
	const float Level,
	const float Knockback, 
	FGameplayTagContainer EffectTags)
{
	if (!DamageEffectClass)
	{
		DamageEffectClass = UBSDamageEffect::StaticClass();
	}
	
	FFactionTargetMode NewTargetMode = FFactionTargetMode(bHitSelf, bHitEnemies, bHitAllies);
	
	if (Instigator->HasMatchingGameplayTag(BSExtendedAttributeTags::Player_PVPFlagged) && 
		Target->HasMatchingGameplayTag(BSExtendedAttributeTags::Player_PVPFlagged))
	{
		if (!NewTargetMode.EvaluateFactionPvp(Instigator, Target)) return false;
	}
	else if (!NewTargetMode.EvaluateFaction(Instigator, Target)) return false;
	
	const FGameplayEffectSpecHandle SpecHandle = 
		Instigator->MakeOutgoingSpec(DamageEffectClass, Level, Instigator->MakeEffectContext());
	
	if (CustomOrigin)
	{
		SpecHandle.Data->GetContext().AddOrigin(Origin);
	}
	else if (Instigator->GetAvatarActor())
	{
		SpecHandle.Data->GetContext().AddOrigin(Instigator->GetAvatarActor()->GetActorLocation());
	}
	
	for (auto DynamicAssetTags : EffectTags)
	{
		SpecHandle.Data->AddDynamicAssetTag(DynamicAssetTags);
	}
	
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(DefaultSetByCallerTags::SetByCaller_Damage, Damage);
	
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(BSExtendedAttributeTags::Attribute_Knockback, Knockback);
	
	Instigator->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, Target);
	
	return true;
}

bool UBSGameplayLibrary::ApplyHealingTo(
	UAbilitySystemComponent* Instigator, 
	UAbilitySystemComponent* Target,
	const bool CustomOrigin, 
	const FVector Origin,
	const bool bHitSelf,
	const bool bHitAllies,
	const bool bHitEnemies,
	TSubclassOf<UBSHealingEffect> HealingEffectClass, 
	const float Healing, 
	const float Level, 
	const float Knockback,
	FGameplayTagContainer EffectTags)
{
	if (!HealingEffectClass)
	{
		HealingEffectClass = UBSHealingEffect::StaticClass();
	}
	
	FFactionTargetMode NewTargetMode = FFactionTargetMode(bHitSelf, bHitEnemies, bHitAllies);
	
	if (Instigator->HasMatchingGameplayTag(BSExtendedAttributeTags::Player_PVPFlagged) && 
		Target->HasMatchingGameplayTag(BSExtendedAttributeTags::Player_PVPFlagged))
	{
		if (!NewTargetMode.EvaluateFactionPvp(Instigator, Target)) return false;
	}
	else if (!NewTargetMode.EvaluateFaction(Instigator, Target)) return false;
	
	const FGameplayEffectSpecHandle SpecHandle = 
		Instigator->MakeOutgoingSpec(HealingEffectClass, Level, Instigator->MakeEffectContext());
	
	if (CustomOrigin)
	{
		SpecHandle.Data->GetContext().AddOrigin(Origin);
	}
	else if (Instigator->GetAvatarActor())
	{
		SpecHandle.Data->GetContext().AddOrigin(Instigator->GetAvatarActor()->GetActorLocation());
	}
	
	for (auto DynamicAssetTags : EffectTags)
	{
		SpecHandle.Data->AddDynamicAssetTag(DynamicAssetTags);
	}
	
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(DefaultSetByCallerTags::SetByCaller_Healing, Healing);
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(BSExtendedAttributeTags::Attribute_Knockback, Knockback);
	
	Instigator->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, Target);
	
	return true;
}

bool UBSGameplayLibrary::ApplyBuffTo(
	UAbilitySystemComponent* Instigator, 
	UAbilitySystemComponent* Target,
	const bool CustomOrigin, 
	const FVector Origin,
	const bool bHitSelf,
	const bool bHitAllies,
	const bool bHitEnemies,
	TSubclassOf<UBSBuffEffect> BuffEffectClass,
	const float Duration, 
	const float Level, 
	FGameplayTagContainer EffectTags)
{
	if (Duration <= 0) return false;
	
	if (!BuffEffectClass)
	{
		BuffEffectClass = UBSBuffEffect::StaticClass();
	}
	
	FFactionTargetMode NewTargetMode = FFactionTargetMode(bHitSelf, bHitEnemies, bHitAllies);
	
	if (Instigator->HasMatchingGameplayTag(BSExtendedAttributeTags::Player_PVPFlagged) && 
		Target->HasMatchingGameplayTag(BSExtendedAttributeTags::Player_PVPFlagged))
	{
		if (!NewTargetMode.EvaluateFactionPvp(Instigator, Target)) return false;
	}
	else if (!NewTargetMode.EvaluateFaction(Instigator, Target)) return false;
	
	const FGameplayEffectSpecHandle SpecHandle = 
		Instigator->MakeOutgoingSpec(BuffEffectClass, Level, Instigator->MakeEffectContext());
	
	if (CustomOrigin)
	{
		SpecHandle.Data->GetContext().AddOrigin(Origin);
	}
	else if (Instigator->GetAvatarActor())
	{
		SpecHandle.Data->GetContext().AddOrigin(Instigator->GetAvatarActor()->GetActorLocation());
	}
	
	for (auto DynamicAssetTags : EffectTags)
	{
		SpecHandle.Data->AddDynamicAssetTag(DynamicAssetTags);
	}
	
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(DefaultSetByCallerTags::SetByCaller_Duration, Duration);
	
	Instigator->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, Target);
	
	return true;
}
