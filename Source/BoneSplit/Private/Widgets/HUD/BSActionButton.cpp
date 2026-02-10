// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSActionButton.h"

#include "AbilitySystemComponent.h"
#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "IconThumbnailInterface.h"

void UBSActionButton::InitializeActionButton(UAbilitySystemComponent* InAbilitySystemComponent)
{
	check(InAbilitySystemComponent);
	
	AbilitySystemComponent = InAbilitySystemComponent;
	
	if (AbilityIcon->GetDynamicMaterial())
	{
		AbilityIcon->GetDynamicMaterial()->SetScalarParameterValue("Percent", 1);
	}
}

void UBSActionButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!IsValid(this)) return;
	if (!GetButtonMaterial()) return;
	if (!GetAbilitySystemComponent()) return;
	
	const FGameplayAbilitySpec* Spec = GetAbilitySpecForID();
	if (!Spec) return;
	UGameplayAbility* AbilityInstance = Spec->GetPrimaryInstance();
	if (!AbilityInstance) return;
			
	GetButtonMaterial()->SetScalarParameterValue(PressedParamName, Spec->InputPressed ? 1 : 0);
				
	float TimeRemaining = 0;
	float Duration = 0;
	
	AbilityInstance->GetCooldownTimeRemainingAndDuration(
		Spec->Handle,
		GetAbilitySystemComponent()->AbilityActorInfo.Get(),
		TimeRemaining,
		Duration
	);
	
	if (AbilityInstance->GetCooldownGameplayEffect() && AbilityInstance->GetCooldownGameplayEffect()->StackLimitCount > 1)
	{
		const int32 CurrentStacks = GetAbilitySystemComponent()->GetGameplayEffectCount(AbilityInstance->GetCooldownGameplayEffect()->GetClass(), nullptr);
		const int32 MaxStacks = AbilityInstance->GetCooldownGameplayEffect()->StackLimitCount;
		
		AbilityChargeText->SetText(FText::FromString(FString::FromInt(MaxStacks - CurrentStacks)));
		AbilityChargeText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		AbilityChargeText->SetVisibility(ESlateVisibility::Hidden);
	}

	const bool bCanAfford = AbilityInstance->CheckCost(Spec->Handle, AbilityInstance->GetCurrentActorInfo());
			
	GetButtonMaterial()->SetScalarParameterValue(CanActivateParam, bCanAfford ? 1 : 0);
				
	AbilityIcon->GetDynamicMaterial()->SetScalarParameterValue(
		PercentParamName, Duration > 0 ? 1 - TimeRemaining / Duration : 1);
}

void UBSActionButton::Test(UGameplayAbility* AbilityInstance, float& TimeRemaining, float& CooldownDuration)
{
	
	TimeRemaining = 0.f;
	CooldownDuration = 0.f;
	
	const FGameplayTagContainer* CooldownTags = AbilityInstance->GetCooldownTags();
	if (CooldownTags && CooldownTags->Num() > 0)
	{
		if (GetAbilitySystemComponent())
		{
			FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*CooldownTags);
			TArray<TPair<float, float>> DurationAndTimeRemaining = GetAbilitySystemComponent()->GetActiveEffectsTimeRemainingAndDuration(Query);
			if (DurationAndTimeRemaining.Num() > 0)
			{
				int32 BestIdx = 0;
				float ShortestTime = DurationAndTimeRemaining[0].Key;
				for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
				{
					if (DurationAndTimeRemaining[Idx].Key < ShortestTime)
					{
						ShortestTime = DurationAndTimeRemaining[Idx].Key;
						BestIdx = Idx;
					}
				}

				TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
				CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;
			}
		}
	}
}

UAbilitySystemComponent* UBSActionButton::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.IsValid() ? AbilitySystemComponent.Get() : nullptr;
}

UMaterialInstanceDynamic* UBSActionButton::GetButtonMaterial() const
{
	return AbilityIcon->GetDynamicMaterial();
}

FGameplayAbilitySpec* UBSActionButton::GetAbilitySpecForID()
{
	FGameplayAbilitySpec* CurrentSpec = AbilitySystemComponent.Get()->FindAbilitySpecFromHandle(CurrentCachedHandle);
	
	if (!CurrentSpec)
	{
		CurrentSpec = AbilitySystemComponent.Get()->FindAbilitySpecFromInputID(InputID);
		if (!CurrentSpec) return nullptr;
		CurrentCachedHandle = CurrentSpec->Handle;
		
		//Also update the icon when the spec changed.
		
		if (const UGameplayAbility* AbilityInstance = CurrentSpec->GetPrimaryInstance(); 
			AbilityInstance && AbilityInstance->Implements<UIconThumbnailInterface>())
		{
			if (!IIconThumbnailInterface::Execute_GetIcon(AbilityInstance).IsNull())
			{
				if (UTexture2D* LoadedIcon = 
					IIconThumbnailInterface::Execute_GetIcon(AbilityInstance).LoadSynchronous())
				{
					AbilityIcon->GetDynamicMaterial()->SetTextureParameterValue(TextureParamName, LoadedIcon);
				}
			}
		}
	}
	
	return CurrentSpec;
}
