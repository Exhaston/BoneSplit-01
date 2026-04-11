// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Abilities/BSExtendedAttributeSet.h"

#include "AbilitiesExtensionLib.h"
#include "CharacterAbilitySystem.h"
#include "GameplayEffectExtension.h"
#include "Abilities/BSThornsEffect.h"
#include "BoneSplit/BoneSplit.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/Abilities/BSPlayerAbilityBase_Montage.h"
#include "Player/HUD/BSWidget_FloatingText.h"
#include "Threat/MThreatComponent.h"
#include "Threat/MThreatInterface.h"

namespace BSExtendedAttributeTags
{
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_SuccessfulBlock, "GameplayEvent.SuccessfulBlock");
	
	UE_DEFINE_GAMEPLAY_TAG(Status_God, "Status.God");
	
	UE_DEFINE_GAMEPLAY_TAG(Effect_HealthLeeching, "Effect.HealthLeeching");
	UE_DEFINE_GAMEPLAY_TAG(Effect_ManaLeeching, "Effect.ManaLeeching");
	UE_DEFINE_GAMEPLAY_TAG(Effect_SoulCharging, "Effect.SoulCharging");
	UE_DEFINE_GAMEPLAY_TAG(Effect_NotBlockable, "Effect.NotBlockable");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_RawMagnitude, "Effect.RawMagnitude", "Damage effects with this tag will skip critical strikes and scaling all together");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Type_Magic, "Effect.Type.Magic", "Damage effects with this tag will be able to be mitigated using Magic Resist");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Type_Physical, "Effect.Type.Physical", "Damage effects with this tag will be able to be mitigated by Armor");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_NoRetaliate, "Effect.NoRetaliate", "Damage effects with this tag will not trigger thorns or other recursive effects");
	
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Knockback, "Attribute.Knockback");
	UE_DEFINE_GAMEPLAY_TAG(Player_PVPFlagged, "Player.PVPFlagged");
}

void UBSExtendedAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, Power, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, CriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, CriticalModifier, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, CurrentMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, RangeModifier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, MultiHit, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, Thorns, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, Leech, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, ManaLeech, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, AbilityCostModifier, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, Friction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, JumpHeight, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, SoulCharge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, SoulChargeAmount, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, CooldownReductionModifier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, DamageModifier, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBSExtendedAttributeSet, MagicResist, COND_None, REPNOTIFY_Always);
}

void UBSExtendedAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetKnockbackAttribute() && GetKnockback() != 0)
	{
		const AActor* SourceActor = Data.EffectSpec.GetContext().GetEffectCauser();
		
		const FVector Origin = 
			Data.EffectSpec.GetContext().HasOrigin() ? 
		Data.EffectSpec.GetContext().GetOrigin() : SourceActor ? 
		SourceActor->GetActorLocation() : FVector::ZeroVector;
		
		float LocalKnockback = GetKnockback();
		SetKnockback(0);
		HandleKnockback(GetOwningAbilitySystemComponent(), LocalKnockback, Origin);
	}
}

void UBSExtendedAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(
			GetOwningAbilitySystemComponent(), GetCurrentManaAttribute(), OldValue, NewValue);
	}
}

float UBSExtendedAttributeSet::GetMaxForAttribute(const FGameplayAttribute& Attribute) const
{
	if (Attribute == GetCurrentManaAttribute() && GetMaxMana() > 0) return GetMaxMana();
	if (Attribute == GetSoulChargeAttribute() && GetMaxSoulCharge() > 0) return GetMaxSoulCharge();
	return Super::GetMaxForAttribute(Attribute);
}

float UBSExtendedAttributeSet::GetMinForAttribute(const FGameplayAttribute& Attribute) const
{
	if (Attribute == GetCriticalModifierAttribute()) return 1;
	if (Attribute == GetCriticalChanceAttribute()) return 0;
	if (Attribute == GetCurrentManaAttribute()) return 0;
	return Super::GetMinForAttribute(Attribute);
}

void UBSExtendedAttributeSet::HandleKnockback(UAbilitySystemComponent* TargetAsc, float& KnockbackToApply, const FVector Origin)
{
	if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetAsc->GetAvatarActor()))
	{
		if (FMath::IsNearlyZero(KnockbackToApply)) return;

		const FVector Direction = (TargetAsc->GetAvatarActor()->GetActorLocation() - Origin).GetSafeNormal2D();

		// Horizontal direction respects knockback sign (push away or pull toward)
		FVector LaunchVelocity = Direction * KnockbackToApply;

		// Upward force is always positive regardless of knockback direction
		LaunchVelocity.Z = FMath::Abs(KnockbackToApply) * 0.75f;

		TargetCharacter->LaunchCharacter(LaunchVelocity, true, true);
	}
}

void UBSExtendedAttributeSet::HandleHealing(UCharacterAbilitySystem* InstigatorAsc, UCharacterAbilitySystem* TargetAsc,
                                            float BaseHeal, float& HealToApply, FGameplayTagContainer EffectTags, const FGameplayEffectSpec& EffectSpec)
{
	
	FGameplayEffectSpec EffectCopy = EffectSpec;
	
	float TotalPower = InstigatorAsc->GetNumericAttribute(GetPowerAttribute());
	
	if (const auto PowerAttrMod = EffectCopy.GetModifiedAttribute(GetPowerAttribute()))
	{
		TotalPower += PowerAttrMod->TotalMagnitude;
	}
	
	HealToApply += UAbilitiesExtensionLib::AsymptoticDr(TotalPower, 1000);
	
	float TotalCritChance = InstigatorAsc->GetNumericAttribute(GetCriticalChanceAttribute());
	
	if (const auto CritChanceSpecMod = EffectCopy.GetModifiedAttribute(GetCriticalChanceAttribute()))
	{
		TotalCritChance += CritChanceSpecMod->TotalMagnitude;
	}
	
	if (UKismetMathLibrary::RandomBoolWithWeight(FMath::Clamp(TotalCritChance, 0.f, 1.f)))
	{
		float TotalCritMod = InstigatorAsc->GetNumericAttribute(GetCriticalModifierAttribute());
	
		if (const auto CritModSpecMod = EffectCopy.GetModifiedAttribute(GetCriticalModifierAttribute()))
		{
			TotalCritMod += CritModSpecMod->TotalMagnitude;
		}
		
		HealToApply *= TotalCritMod;
		EffectTags.AddTag(BSTags::EffectTag_Critical);
	}
	
	Super::HandleHealing(InstigatorAsc, TargetAsc, BaseHeal, HealToApply, EffectTags, EffectSpec);
}

void UBSExtendedAttributeSet::HandleDamage(UCharacterAbilitySystem* InstigatorAsc, UCharacterAbilitySystem* TargetAsc,
	float BaseDamage, float& DamageToApply, FGameplayTagContainer EffectTags, const FGameplayEffectSpec& EffectSpec)
{
	
	if (!IsValid(TargetAsc->GetAvatarActor())) return;
	
	if (!EffectTags.HasTagExact(BSExtendedAttributeTags::Effect_NotBlockable) && 
		UKismetMathLibrary::RandomBoolWithWeight(UAbilitiesExtensionLib::AsymptoticDr(GetBlockChance(), 1)))
	{
		FGameplayEventData BlockPayload;
		BlockPayload.ContextHandle = TargetAsc->MakeEffectContext();
		BlockPayload.EventMagnitude = 1;
		BlockPayload.EventTag = BSExtendedAttributeTags::GameplayEvent_SuccessfulBlock;
		BlockPayload.Instigator = InstigatorAsc->GetAvatarActor();
		BlockPayload.Target = TargetAsc->GetAvatarActor();
		TargetAsc->HandleGameplayEvent(BSExtendedAttributeTags::GameplayEvent_SuccessfulBlock, &BlockPayload);
		return;
	}
	
	FGameplayEffectSpec EffectCopy = EffectSpec;
	
	if (!EffectTags.HasTagExact(BSExtendedAttributeTags::Effect_RawMagnitude))
	{
		const float DamageMod = InstigatorAsc->GetNumericAttribute(GetDamageModifierAttribute());
		DamageToApply *= DamageMod;
		
		//Power
		float TotalPower = InstigatorAsc->GetNumericAttribute(GetPowerAttribute());
		DamageToApply += UAbilitiesExtensionLib::AsymptoticDr(TotalPower, 1000);
		
		//Critical Strikes

		const float TotalCritChance = InstigatorAsc->GetNumericAttribute(GetCriticalChanceAttribute());
		const float ClampedCritChance = FMath::Clamp(TotalCritChance, 0.f, 1.f);
		const float ExcessCritChance = FMath::Max(0.f, TotalCritChance - 1.f);
	
		if (UKismetMathLibrary::RandomBoolWithWeight(ClampedCritChance))
		{
			float TotalCritMod = InstigatorAsc->GetNumericAttribute(GetCriticalModifierAttribute());
		
			TotalCritMod = FMath::Clamp(TotalCritMod, 1, FLT_MAX);
		
			DamageToApply *= TotalCritMod * (1.f + ExcessCritChance);
			EffectTags.AddTagFast(BSGameplayTags::Widget_FloatingText_Crit);
			EffectTags.AddTag(BSTags::EffectTag_Critical);
		}        
		
		//Physical Mitigation
		
		if (EffectTags.HasTagExact(BSExtendedAttributeTags::Effect_Type_Physical) && GetArmor() > 0)
		{
			const float TotalArmor = UAbilitiesExtensionLib::AsymptoticDr(GetArmor(), 1);
			DamageToApply /= TotalArmor;
		}
		
		//Magic Resist Mitigation
		
		if (EffectTags.HasTagExact(BSExtendedAttributeTags::Effect_Type_Magic) && GetMagicResist() > 0)
		{
			const float TotalMR = UAbilitiesExtensionLib::AsymptoticDr(GetMagicResist(), 1);
			DamageToApply /= TotalMR;
		}
	}
	
	DamageToApply = FMath::RoundToInt32(DamageToApply);
	
	Super::HandleDamage(InstigatorAsc, TargetAsc, BaseDamage, DamageToApply, EffectTags, EffectSpec);
	
	//Only apply thorns from non thorn damage.
	if (GetThorns() > 0 && !EffectTags.HasTagExact(BSExtendedAttributeTags::Effect_NoRetaliate)) 
	{
		const FGameplayEffectSpecHandle SpecHandle = TargetAsc->MakeOutgoingSpec(
			UBSThornsEffect::StaticClass(), 1, TargetAsc->MakeEffectContext());

		if (!SpecHandle.IsValid()) return;
	
		SpecHandle.Data->SetSetByCallerMagnitude(
			DefaultSetByCallerTags::SetByCaller_Damage,GetThorns()
		);
		
		SpecHandle.Data->AddDynamicAssetTag(BSExtendedAttributeTags::Effect_RawMagnitude);
		SpecHandle.Data->AddDynamicAssetTag(BSExtendedAttributeTags::Effect_NotBlockable);
		
		//Stop this from triggering again from the new effect
		SpecHandle.Data->AddDynamicAssetTag(BSExtendedAttributeTags::Effect_NoRetaliate); 
		
		TargetAsc->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, InstigatorAsc);
	}
	
	if (IMThreatInterface* ThreatInterface = Cast<IMThreatInterface>(TargetAsc->GetAvatarActor()))
	{
		if (UMThreatComponent* ThreatComponent = ThreatInterface->GetThreatComponent())
		{
			ThreatComponent->AddThreat(InstigatorAsc->GetAvatarActor(), DamageToApply); //Lives on the controller, could be null
		}
	}                                        
	
	if (const float SourceLeech = InstigatorAsc->GetNumericAttribute(GetLeechAttribute()); 
		SourceLeech > 0 && EffectTags.HasTagExact(BSExtendedAttributeTags::Effect_HealthLeeching))
	{
		InstigatorAsc->ApplyModToAttribute(GetCurrentHealthAttribute(), 
			EGameplayModOp::AddBase, SourceLeech); 
	}
	
	if (const float SourceManaLeech = InstigatorAsc->GetNumericAttribute(GetManaLeechAttribute()); 
		SourceManaLeech > 0 && EffectTags.HasTagExact(BSExtendedAttributeTags::Effect_ManaLeeching))
	{
		InstigatorAsc->ApplyModToAttribute(GetCurrentManaAttribute(), 
			EGameplayModOp::AddBase, SourceManaLeech); 
	}
	
	if (const float SourceSoulChargeAmount = InstigatorAsc->GetNumericAttribute(GetSoulChargeAmountAttribute()); 
		SourceSoulChargeAmount > 0 && EffectTags.HasTagExact(BSExtendedAttributeTags::Effect_SoulCharging))
	{
		InstigatorAsc->ApplyModToAttribute(GetSoulChargeAttribute(), EGameplayModOp::AddBase, SourceSoulChargeAmount);
	}
}
