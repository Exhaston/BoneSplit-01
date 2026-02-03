// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/BSAttributeBar.h"
#include "CommonTextBlock.h"
#include "CommonUIUtils.h"
#include "Components/Image.h"

//TODO: Was set up quite quickly. Need a thorough cleanup for readability.

void UBSAttributeBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (GetBarMaterial())
	{
		GetBarMaterial()->SetVectorParameterValue("Color", Color);
	}
}

void UBSAttributeBar::NativeConstruct()
{
	Super::NativeConstruct();
	if (GetBarMaterial())
	{
		GetBarMaterial()->SetVectorParameterValue("Color", Color);
	}
}

UMaterialInstanceDynamic* UBSAttributeBar::GetBarMaterial()
{
	if (HealthBarImage && HealthBarImage->GetDynamicMaterial())
	{
		return HealthBarImage->GetDynamicMaterial();
	}
	return nullptr;
}

void UBSAttributeBar::InitializeAttributeBar(UAbilitySystemComponent* Asc)
{
	check(Asc);
	AbilitySystemComponent = Asc;
	
	SetAttributeValues(
		AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute), 
		AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute));
	
	//Init so smooth delta bar doesn't move on init
	SmoothNormalizedPercent = NormalizedPercent; 
	GetBarMaterial()->SetScalarParameterValue("Previous", SmoothNormalizedPercent);
	
	AbilitySystemComponent.Get()->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).AddWeakLambda(
	this, [this](const FOnAttributeChangeData& Data)
	{
		SetAttributeValues(
			AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute), 
			AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute));
	});
	AbilitySystemComponent.Get()->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddWeakLambda(
	this, [this](const FOnAttributeChangeData& Data)
	{
		SetAttributeValues(
			AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute), 
			AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute));
	});
}

void UBSAttributeBar::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (IsValid(this))
	{
		SmoothOldToCurrent(InDeltaTime);
	}
}

void UBSAttributeBar::SmoothOldToCurrent(const float DeltaTime)
{
	if (NormalizedPercent == SmoothNormalizedPercent) return;
	if (GetBarMaterial() && 
		AbilitySystemComponent.IsValid() && 
		CurrentAttribute.IsValid() && 
		MaxAttribute.IsValid())
	{
		SmoothNormalizedPercent = FMath::FInterpTo(SmoothNormalizedPercent, NormalizedPercent, DeltaTime, InterpSpeed);
		GetBarMaterial()->SetScalarParameterValue("Previous", SmoothNormalizedPercent);
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
		GetBarMaterial()->SetScalarParameterValue("Current", InNormalizedPercent);
	}
}
