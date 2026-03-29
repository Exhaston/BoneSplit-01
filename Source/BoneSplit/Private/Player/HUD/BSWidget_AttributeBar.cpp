// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/HUD/BSWidget_AttributeBar.h"

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CommonRichTextBlock.h"
#include "Components/Image.h"

namespace BSConsoleVariables
{
	TAutoConsoleVariable<bool> CVarBSBarsShowNumbers(
		TEXT("BS.NamePlates.ShowNumbers"),
		false,
		TEXT("false = Default"),
		ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarBSBarsShowPercentages(
		TEXT("BS.NamePlates.ShowPercentages"),
		false,
		TEXT("false = Default"),
		ECVF_Default);	
}

void UBSWidget_AttributeBar::SetAbilitySystemToTrack(
	const FGameplayAttribute InCurrentAttribute,
	const FGameplayAttribute InMaxAttribute,
	UAbilitySystemComponent* InAsc)
{
	TrackedAsc = InAsc;
	
	CurrentAttribute = InCurrentAttribute;
	MaxAttribute = InMaxAttribute;
	
	if (!ensure(CurrentAttribute.IsValid())) return;
	if (!ensure(MaxAttribute.IsValid())) return;

	const float Current = 
		InAsc->GetNumericAttribute(CurrentAttribute);
	
	const float Max = 
		InAsc->GetNumericAttribute(MaxAttribute);
	
	if (Max > 0)
	{
		const float Percent = (Current / Max) * 100.f;
    
		ProgressBarPercentText->SetText(
			FText::FromString(FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Percent)))
		);
		
		ProgressBarValueText->SetText(
			FText::FromString(FString::Printf(TEXT("%d / %d"), 
				FMath::RoundToInt(Current), 
				FMath::RoundToInt(Max)))
		);
		
		SetProgress(Current / Max);
	}
	
	InAsc->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).AddUObject(
		this, &UBSWidget_AttributeBar::OnAttributeValueChanged);
	
	InAsc->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(
	this, &UBSWidget_AttributeBar::OnAttributeValueChanged);

}

void UBSWidget_AttributeBar::OnAttributeValueChanged(const FOnAttributeChangeData& NewData)
{
	if (!TrackedAsc.IsValid())  return;

	const float Current = 
		TrackedAsc->GetNumericAttribute(CurrentAttribute);

	const float Max = 
		TrackedAsc->GetNumericAttribute(MaxAttribute);
	
	if (Max > 0)
	{
		const float Percent = (Current / Max) * 100.f;
    
		ProgressBarPercentText->SetText(
			FText::FromString(FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Percent)))
		);
		
		ProgressBarValueText->SetText(
			FText::FromString(FString::Printf(TEXT("%d / %d"), 
				FMath::RoundToInt(Current), 
				FMath::RoundToInt(Max)))
		);
		SetProgress(Current / Max);
	}
}

void UBSWidget_AttributeBar::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
	OldProgress = Progress;
	if (UMaterialInstanceDynamic* BarMat = GetBarMaterial())
	{
		BarMat->SetScalarParameterValue("OldProgress", OldProgress);
	}
}

void UBSWidget_AttributeBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (UMaterialInstanceDynamic* BarMat = GetBarMaterial())
	{
		BarMat->SetVectorParameterValue("FillColor", FVector(FillColor));
		BarMat->SetVectorParameterValue("DeltaColor", FVector(DeltaColor));
	}
}

void UBSWidget_AttributeBar::NativeConstruct()
{
	Super::NativeConstruct();
	
	ProgressBarValueText->SetVisibility(
	BSConsoleVariables::CVarBSBarsShowNumbers.GetValueOnGameThread() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	
	BSConsoleVariables::CVarBSBarsShowNumbers->OnChangedDelegate().AddWeakLambda(
	this, [this](IConsoleVariable* ConsoleVariable)
	{
		ProgressBarValueText->SetVisibility(
			ConsoleVariable->GetBool() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	});
	
	ProgressBarPercentText->SetVisibility(
	BSConsoleVariables::CVarBSBarsShowPercentages.GetValueOnGameThread() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	
	BSConsoleVariables::CVarBSBarsShowPercentages->OnChangedDelegate().AddWeakLambda(
		this, [this](IConsoleVariable* ConsoleVariable)
	{
			ProgressBarPercentText->SetVisibility(
				ConsoleVariable->GetBool() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	});
}

void UBSWidget_AttributeBar::SetProgress(const float NewProgress)
{
	Progress = NewProgress;
	if (OldProgress < Progress)
	{
		OldProgress = Progress;
		if (UMaterialInstanceDynamic* BarMat = GetBarMaterial())
		{
			BarMat->SetScalarParameterValue("OldProgress", OldProgress);
		}
	}
	if (UMaterialInstanceDynamic* BarMat = GetBarMaterial())
	{
		BarMat->SetScalarParameterValue("Progress", NewProgress);
	}
}

void UBSWidget_AttributeBar::SetBonusProgress(float NewBonusProgress)
{
	Progress = NewBonusProgress;
			
	if (UMaterialInstanceDynamic* BarMat = GetBarMaterial())
	{
		BarMat->SetScalarParameterValue("BonusProgress", NewBonusProgress);
	}
}

void UBSWidget_AttributeBar::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (OldProgress != Progress)
	{
		OldProgress = FMath::FInterpTo(OldProgress, Progress, InDeltaTime, InterpSpeed);
		
		if (UMaterialInstanceDynamic* BarMat = GetBarMaterial())
		{
			BarMat->SetScalarParameterValue("OldProgress", OldProgress);
		}
	}
}

UMaterialInstanceDynamic* UBSWidget_AttributeBar::GetBarMaterial()
{
	return ProgressBarImage ? ProgressBarImage->GetDynamicMaterial() : nullptr;
}
