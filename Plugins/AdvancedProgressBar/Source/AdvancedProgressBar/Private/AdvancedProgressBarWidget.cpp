// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "AdvancedProgressBarWidget.h"

#include "Components/Image.h"

void UAdvancedProgressBarWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (UMaterialInstanceDynamic* BarMat = GetBarMaterial())
	{
		BarMat->SetVectorParameterValue("FillColor", FVector(FillColor));
		BarMat->SetVectorParameterValue("DeltaColor", FVector(DeltaColor));
	}
}

void UAdvancedProgressBarWidget::SetProgress(const float NewProgress)
{
	Progress = NewProgress;
	if (UMaterialInstanceDynamic* BarMat = GetBarMaterial())
	{
		BarMat->SetScalarParameterValue("Progress", NewProgress);
	}
}

void UAdvancedProgressBarWidget::SetBonusProgress(float NewBonusProgress)
{
	Progress = NewBonusProgress;
			
	if (UMaterialInstanceDynamic* BarMat = GetBarMaterial())
	{
		BarMat->SetScalarParameterValue("BonusProgress", NewBonusProgress);
	}
}

void UAdvancedProgressBarWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (OldProgress != Progress)
	{
		OldProgress = FMath::FInterpTo(OldProgress, Progress, InDeltaTime, DeltaInterpSpeed);
		
		if (UMaterialInstanceDynamic* BarMat = GetBarMaterial())
		{
			BarMat->SetScalarParameterValue("OldProgress", OldProgress);
		}
	}
}

UMaterialInstanceDynamic* UAdvancedProgressBarWidget::GetBarMaterial()
{
	return ProgressBarImage ? ProgressBarImage->GetDynamicMaterial() : nullptr;
}
