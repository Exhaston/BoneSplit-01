// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSActionButton.h"

#include "AbilityBufferComponent.h"
#include "AbilitySystemComponent.h"
#include "CharacterAbilitySystem.h"
#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "IconThumbnailInterface.h"
#include "BoneSplit/BoneSplit.h"
#include "Player/BSPlayerControllerBase.h"


void UBSActionButton::InitializeActionButton(UCharacterAbilitySystem* InAbilitySystemComponent)
{
	AbilitySystemComponent = InAbilitySystemComponent;

}

void UBSActionButton::NativeConstruct()
{
	Super::NativeConstruct();

	const ABSPlayerControllerBase* PC = GetOwningPlayer<ABSPlayerControllerBase>();
	
	AbilitySystemComponent = Cast<UCharacterAbilitySystem>(PC->GetAbilitySystemComponent());
	
	if (!AbilitySystemComponent)
	{
		const FString MissingAscInfo = "No Ability System found from owning player state. " + GetName();
		UE_LOG(BoneSplit, Warning, TEXT("%s"), *MissingAscInfo);
		return;
	}
	
	AbilityBufferComponent = UAbilityBufferComponent::GetBufferComponentFromController(GetOwningPlayer());
	
	AbilitySystemComponent->NotifyAbilitiesTo(FOnAbilityGranted::FDelegate::CreateWeakLambda(
	this, [this](FGameplayAbilitySpec& Spec)
	{
		AttemptSetNewSpec(&Spec);
	}));
	
	/*
	CVarToggleCooldownNumbers->OnChangedDelegate().AddWeakLambda(this, [this](IConsoleVariable* Var)
	{
		CooldownNumberText->SetVisibility(Var->GetBool() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	});
	
	CooldownNumberText->SetVisibility(CVarToggleCooldownNumbers.GetValueOnGameThread() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	*/
}

bool UBSActionButton::AttemptSetNewSpec(FGameplayAbilitySpec* Spec)
{
	if (!Spec || Spec->InputID != InputID) return false;
	
	CurrentCachedHandle = Spec->Handle;
	
	ResetActionButton();
	
	const UGameplayAbility* AbilityInstance = 
		Spec->GetPrimaryInstance();
	
	TrySetAbilityIcon(AbilityInstance);
	
	return true;
}

void UBSActionButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!GetButtonMaterial() || !AbilitySystemComponent) return;
	
	const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(CurrentCachedHandle);
	if (!Spec)
	{
		DisplayFallbackIcon();
		return;
	}
	
	const UGameplayAbility* AbilityInstance = Spec->GetPrimaryInstance();
	
	if (AbilityBufferComponent)
	{
		GetButtonMaterial()->SetScalarParameterValue(
			PressedParamName, AbilityBufferComponent->GetIsBufferedOrPressed(Spec->InputID));
	}
	
	float TimeRemaining = 0;
	float Duration = 0;
	
	AbilityInstance->GetCooldownTimeRemainingAndDuration(
		Spec->Handle,
		AbilitySystemComponent.Get()->AbilityActorInfo.Get(),
		TimeRemaining,
		Duration
	);
	
	if (TimeRemaining > 0)
	{
		CooldownNumberText->SetRenderOpacity(1);
		
		FNumberFormattingOptions NumberFormat;
		NumberFormat.MinimumFractionalDigits = 0;
		NumberFormat.MaximumFractionalDigits = 0;
		NumberFormat.MinimumIntegralDigits = 1;
		NumberFormat.MaximumIntegralDigits = 10000;
		CooldownNumberText->SetText(FText::AsNumber(TimeRemaining, &NumberFormat));
	}
	else
	{
		CooldownNumberText->SetRenderOpacity(0);
	}

	
	if (AbilityInstance->GetCooldownGameplayEffect() && AbilityInstance->GetCooldownGameplayEffect()->StackLimitCount > 1)
	{
		const int32 CurrentStacks = AbilitySystemComponent.Get()->GetGameplayEffectCount(AbilityInstance->GetCooldownGameplayEffect()->GetClass(), nullptr);
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
				
	GetButtonMaterial()->SetScalarParameterValue(PercentParamName, Duration > 0 ? 1 - TimeRemaining / Duration : 1);
}
UMaterialInstanceDynamic* UBSActionButton::GetButtonMaterial() const
{
	return AbilityIcon->GetDynamicMaterial();
}

void UBSActionButton::DisplayFallbackIcon()
{
	GetButtonMaterial()->SetTextureParameterValue(TextureParamName, FallbackIcon);
	AbilityChargeText->SetVisibility(ESlateVisibility::Hidden);
	GetButtonMaterial()->SetScalarParameterValue(CanActivateParam, 1);
	GetButtonMaterial()->SetScalarParameterValue(PercentParamName, 1);
}

void UBSActionButton::ResetActionButton()
{
	if (GetButtonMaterial())
	{
		GetButtonMaterial()->SetScalarParameterValue("Percent", 1);
	}
}

void UBSActionButton::TrySetAbilityIcon(const UGameplayAbility* AbilityInstance)
{
	if (GetButtonMaterial() && AbilityInstance && AbilityInstance->Implements<UIconThumbnailInterface>())
	{
		if (const TSoftObjectPtr<UTexture2D> SoftIcon = IIconThumbnailInterface::Execute_GetIcon(AbilityInstance); 
			!SoftIcon.IsNull())
		{
			GetButtonMaterial()->SetTextureParameterValue(TextureParamName, 
				IIconThumbnailInterface::Execute_GetIcon(AbilityInstance).LoadSynchronous());
		}
	}
}
