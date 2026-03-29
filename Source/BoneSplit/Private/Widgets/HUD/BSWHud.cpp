// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSWHud.h"

#include "CharacterAbilitySystem.h"
#include "CommonLazyImage.h"
#include "Abilities/BSBuffEffect.h"
#include "Player/Abilities/BSPlayerAbilityBase_HookShot.h"
#include "Widgets/BSLocalWidgetSubsystem.h"
#include "Widgets/HUD/BSAttributeBar.h"
#include "Widgets/HUD/BSActionButton.h"

void UBSWHud::InitializePlayerHUD(UCharacterAbilitySystem* InAbilitySystemComponent)
{
	AbilitySystemComponent = InAbilitySystemComponent;
	if (AbilitySystemComponent.IsValid())
	{
		if (HealthBar) HealthBar->SetAttributeOwner(InAbilitySystemComponent);
	
		if (ManaBar) ManaBar->SetAttributeOwner(InAbilitySystemComponent);
	
		if (SoulBar) SoulBar->SetAttributeOwner(InAbilitySystemComponent);
		
		if (ActionButton1) ActionButton1->InitializeActionButton(InAbilitySystemComponent);
		
		if (ActionButton2) ActionButton2->InitializeActionButton(InAbilitySystemComponent);
		
		if (ActionButton3) ActionButton3->InitializeActionButton(InAbilitySystemComponent);
		
		if (ActionButton4) ActionButton4->InitializeActionButton(InAbilitySystemComponent);
		
		if (ActionButton5) ActionButton5->InitializeActionButton(InAbilitySystemComponent);
		
		/*
		AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddWeakLambda(this, [this]
			(UAbilitySystemComponent* Asc, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
		{
			if (!Spec.Def.IsA(UBSBuffEffect::StaticClass())) return;
			const UBSBuffEffect* SourceEffect = Cast<UBSBuffEffect>(Spec.Def);
			
			if (!SourceEffect) return;
			
			UBSBuffIcon* EffectIcon = BuffMap.FindOrAdd(SourceEffect);
			
			if (!EffectIcon)
			{
				EffectIcon = CreateWidget<UBSBuffIcon>(this, BuffIconClass);
			}
			
			BuffParentSlot->AddChild(EffectIcon);
			EffectIcon->InitializeBuffIcon(SourceEffect);
		});
		
		AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddWeakLambda(this, [this]
			(const FActiveGameplayEffect& EndedEffect)
		{
			if (!EndedEffect.Spec.Def.IsA(UBSBuffEffect::StaticClass())) return;
			const UBSBuffEffect* SourceEffect = Cast<UBSBuffEffect>(EndedEffect.Spec.Def);
			if (BuffMap[SourceEffect])
			{
				BuffMap[SourceEffect]->RemoveFromParent();
			}
			BuffMap.Remove(SourceEffect);
		});
		*/
	}
}

void UBSWHud::NativePreConstruct()
{
	Super::NativePreConstruct();
	
}

void UBSWHud::NativeConstruct()
{
	Super::NativeConstruct();
	

}

void UBSWHud::NativeOnActivated()
{
	Super::NativeOnActivated();
	
}

void UBSWHud::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (AbilitySystemComponent.IsValid())
	{
		TArray<FGameplayAbilitySpec> SpecHandles = AbilitySystemComponent->GetActivatableAbilities();

		for (auto& SpecHandle : SpecHandles)
		{
			UGameplayAbility* AbilityInstance = SpecHandle.GetPrimaryInstance();
			if (!AbilityInstance) continue;
			if (AbilityInstance->IsA(UBSPlayerAbilityBase_HookShot::StaticClass()))
			{
				const bool CanActivate = 
					AbilityInstance->CommitCheck(
						AbilityInstance->GetCurrentAbilitySpecHandle(), 
						AbilityInstance->GetCurrentActorInfo(), 
						AbilityInstance->GetCurrentActivationInfoRef());
				
				HookCrosshairs->SetVisibility(CanActivate ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
				break;
			}
		}
	}
}
