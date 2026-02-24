// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSAttributeBar.h"
#include "CommonTextBlock.h"
#include "CommonUIUtils.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/Image.h"

void UBSAttributeBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (GetBarMaterial()) GetBarMaterial()->SetVectorParameterValue(HealthBarColorParamName, Color);
	
	if (IsDesignTime()) //Mainly for editor preview. If we have a valid asc just use that instead.
	{
		if (GetBarMaterial())
		{
			GetBarMaterial()->SetScalarParameterValue(HealthBarPercentParamName, 0.5);
			GetBarMaterial()->SetScalarParameterValue(HealthBarOldPercentParamName, 0.75);
		}
		
		if (HealthBarPercentText)
		{
			HealthBarPercentText->SetText(FText::FromString("50%"));
		}
	
		if (HealthBarPercentText)
		{
			HealthBarText->SetText(FText::FromString("50/100"));
		}
	}
}

void UBSAttributeBar::NativeConstruct()
{
	Super::NativeConstruct();

	const bool bShouldDisplayNumbers = BSConsoleVariables::CVarBSBarsShowNumbers.GetValueOnGameThread();
	const bool bShouldDisplayPercent = BSConsoleVariables::CVarBSBarsShowPercentages.GetValueOnGameThread();
	
	HealthBarText->SetVisibility(bShouldDisplayNumbers ? 
		ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	
	HealthBarPercentText->SetVisibility(bShouldDisplayPercent ? 
		ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	
	BSConsoleVariables::CVarBSBarsShowNumbers->OnChangedDelegate().AddWeakLambda(
	this,[this] (IConsoleVariable* Variable)
	{
		HealthBarText->SetVisibility(Variable->GetBool() ? 
		ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	});	
	
	BSConsoleVariables::CVarBSBarsShowPercentages->OnChangedDelegate().AddWeakLambda(
	this,[this] (IConsoleVariable* Variable)
	{
		HealthBarPercentText->SetVisibility(Variable->GetBool() ? 
		ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	});
}

void UBSAttributeBar::RemoveFromParent()
{
	if (UAbilitySystemComponent* Asc = AbilitySystemComponent.IsValid() ? AbilitySystemComponent.Get() : nullptr; Asc)
	{
		Asc->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).RemoveAll(this);
		Asc->GetGameplayAttributeValueChangeDelegate(MaxAttribute).RemoveAll(this);
	}

	Super::RemoveFromParent();
}

UMaterialInstanceDynamic* UBSAttributeBar::GetBarMaterial()
{
	if (HealthBarImage && HealthBarImage->GetDynamicMaterial())
	{
		return HealthBarImage->GetDynamicMaterial();
	}
	return nullptr;
}

void UBSAttributeBar::SetAttributeOwner(UAbilitySystemComponent* Asc)
{
	AbilitySystemComponent = Asc;
	
	if (!AbilitySystemComponent.IsValid())
	{
		const FString DebugString = GetName() + ": SetAttributeOwner, Ability system was null";
		UE_LOG(BoneSplit, Warning, TEXT("%s"), *DebugString);
		return;
	}
	
	SetAttributeValues(
		AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute), 
		AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute));
	
	//Init so smooth delta bar doesn't move on init
	SmoothNormalizedPercent = NormalizedPercent; 
	GetBarMaterial()->SetScalarParameterValue(HealthBarOldPercentParamName, SmoothNormalizedPercent);
	
	AbilitySystemComponent.Get()->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).AddWeakLambda(
	this, [this](const FOnAttributeChangeData& Data)
	{
		if (!IsValid(this) || !AbilitySystemComponent.IsValid()) return;
		SetAttributeValues(
			AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute), 
			AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute));
	});
	
	AbilitySystemComponent.Get()->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddWeakLambda(
	this, [this](const FOnAttributeChangeData& Data)
	{
		if (!IsValid(this) || !AbilitySystemComponent.IsValid()) return;
		SetAttributeValues(
			AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute), 
			AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute));
	});
}

void UBSAttributeBar::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (AbilitySystemComponent.IsValid())
	{
		SmoothOldToCurrent(InDeltaTime);
	}
}

void UBSAttributeBar::SmoothOldToCurrent(const float DeltaTime)
{
	if (FMath::IsNearlyEqual(NormalizedPercent, SmoothNormalizedPercent)) return; 
	
	if (GetBarMaterial() &&                   
		AbilitySystemComponent.IsValid() && 
		CurrentAttribute.IsValid() && 
		MaxAttribute.IsValid())
	{
		SmoothNormalizedPercent = FMath::FInterpTo(SmoothNormalizedPercent, NormalizedPercent, DeltaTime, InterpSpeed);
		GetBarMaterial()->SetScalarParameterValue(HealthBarOldPercentParamName, SmoothNormalizedPercent);
	}
}

void UBSAttributeBar::SetAttributeValues(const float InCurrentValue, const float InMaxValue)
{
	Value = FMath::Clamp(InCurrentValue, 0, FLT_MAX);
	MaxValue = FMath::Clamp(InMaxValue, 1, FLT_MAX);
	NormalizedPercent = InCurrentValue / InMaxValue; 

	SetTextElements(FMath::RoundToInt(Value), FMath::RoundToInt(MaxValue), NormalizedPercent);
	SetBarProgress(NormalizedPercent);
}

void UBSAttributeBar::SetTextElements(const int32 InCurrentValue, const int32 InMaxValue, const float InNormalizedPercent)
{
	const FString CurrentNum = FString::FromInt(InCurrentValue);
	const FString CurrentMaxNum = FString::FromInt(InMaxValue);
	const FString Percent = FString::FromInt(FMath::RoundToInt(InNormalizedPercent * 100));
	
	HealthBarText->SetText(FText::FromString(CurrentNum + "/" + CurrentMaxNum));
	HealthBarPercentText->SetText(FText::FromString(Percent + "%"));
}

void UBSAttributeBar::SetBarProgress(const float InNormalizedPercent)
{
	if (GetBarMaterial())
	{
		GetBarMaterial()->SetScalarParameterValue(HealthBarPercentParamName, InNormalizedPercent);
	}
}
