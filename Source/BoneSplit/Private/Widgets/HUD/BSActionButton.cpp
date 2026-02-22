// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSActionButton.h"

#include "AbilitySystemComponent.h"
#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "IconThumbnailInterface.h"
#include "Actors/Player/BSPlayerState.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"

void UBSActionButton::NativeConstruct()
{
	Super::NativeConstruct();
	const ABSPlayerState* PS = GetOwningPlayerState<ABSPlayerState>();
	if (!PS)
	{
		const FString MissingAscInfo = "No Owning Player State found. " + GetName();
		UE_LOG(BoneSplit, Warning, TEXT("%s"), *MissingAscInfo);
		return;
	}
	AbilitySystemComponent = PS->GetBSAbilitySystem();
	if (!AbilitySystemComponent.IsValid())
	{
		const FString MissingAscInfo = "No Ability System found from owning player state. " + GetName();
		UE_LOG(BoneSplit, Warning, TEXT("%s"), *MissingAscInfo);
		return;
	}
	
	TArray<FGameplayAbilitySpecHandle> CurrentSpecHandles;
	AbilitySystemComponent->FindAllAbilitiesWithTags(CurrentSpecHandles, FGameplayTagContainer(AbilityTag));

	for (const auto& SpecHandle : CurrentSpecHandles)
	{

		AttemptSetNewSpec(AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle));
	}
	
	AbilitySystemComponent->NotifyAbilitiesTo(FBSOnAbilityGranted::FDelegate::CreateWeakLambda(
	this, [this](FGameplayAbilitySpec& Spec)
	{
		AttemptSetNewSpec(&Spec);
	}));
}

bool UBSActionButton::AttemptSetNewSpec(FGameplayAbilitySpec* Spec)
{
	if (!Spec || !Spec->Ability || !Spec->Ability->GetAssetTags().HasTagExact(AbilityTag)) return false;
	
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
	
	if (!GetButtonMaterial() || !AbilitySystemComponent.IsValid()) return;
	
	const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(CurrentCachedHandle);
	if (!Spec)
	{
		DisplayFallbackIcon();
		return;
	}
	
	const UGameplayAbility* AbilityInstance = Spec->GetPrimaryInstance();
			
	GetButtonMaterial()->SetScalarParameterValue(PressedParamName, Spec->InputPressed ? 1 : 0);
				
	float TimeRemaining = 0;
	float Duration = 0;
	
	AbilityInstance->GetCooldownTimeRemainingAndDuration(
		Spec->Handle,
		AbilitySystemComponent.Get()->AbilityActorInfo.Get(),
		TimeRemaining,
		Duration
	);
	
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
