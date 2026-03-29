// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "UserSettings/BSWidget_IntSelector.h"

#include "CommonRichTextBlock.h"
#include "Components/Image.h"

void UBSWidget_IntSelector::NativeOnClicked()
{
	Super::NativeOnClicked();
	
	const APlayerController* PC = GetOwningPlayer();
	float MouseX, MouseY;
	PC->GetMousePosition(MouseX, MouseY);
	FVector2D MouseScreenPos(MouseX, MouseY);

	// Convert screen pos → local widget space
	FVector2D LocalMousePos = GetCachedGeometry().AbsoluteToLocal(MouseScreenPos);

	const float HalfWidth = GetDesiredSize().X * 0.5f;

	if (LocalMousePos.X < HalfWidth) Decrement();
	else Increment();
}

void UBSWidget_IntSelector::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	UpdateDisplay();
}

void UBSWidget_IntSelector::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (IsHovered() || HasAnyUserFocus())
	{
		const APlayerController* PC = GetOwningPlayer();
		float MouseX, MouseY;
		PC->GetMousePosition(MouseX, MouseY);
		FVector2D MouseScreenPos(MouseX, MouseY);

		// Convert screen pos → local widget space
		FVector2D LocalMousePos = GetCachedGeometry().AbsoluteToLocal(MouseScreenPos);

		const float HalfWidth = GetDesiredSize().X * 0.5f;

		if (LocalMousePos.X < HalfWidth) 
		{
			SelectionImage->GetDynamicMaterial()->SetScalarParameterValue("Selection", 0);
		}
		else
		{
			SelectionImage->GetDynamicMaterial()->SetScalarParameterValue("Selection", 1);
		}
		
		return;
	}
	
	SelectionImage->GetDynamicMaterial()->SetScalarParameterValue("Selection", -1);
}

FNavigationReply UBSWidget_IntSelector::NativeOnNavigation(const FGeometry& MyGeometry,
                                                           const FNavigationEvent& InNavigationEvent,
                                                           const FNavigationReply& InDefaultReply)
{
	const EUINavigation NavType = InNavigationEvent.GetNavigationType();
	if (NavType == EUINavigation::Left)
	{
		Decrement();
		return FNavigationReply::Explicit(nullptr); // consume, don't move focus
	}

	if (NavType == EUINavigation::Right)
	{
		Increment();
		return FNavigationReply::Explicit(nullptr); // consume, don't move focus
	}
	return Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);
}

void UBSWidget_IntSelector::InitIntSelector(
	const int32 InMinValue, const int32 InMaxValue, const TArray<FText> InValueTextDisplays, const bool InCanWrap)
{
	bCanWrap = InCanWrap;
	SetRange(InMinValue, InMaxValue);
	SetDisplayTexts(InValueTextDisplays);
}

void UBSWidget_IntSelector::SetValue(const int32 NewValue)
{
	if (bCanWrap)
	{
		if (NewValue > MaxValue) CurrentValue = MinValue;
		else if (NewValue < MinValue) CurrentValue = MaxValue;
		else CurrentValue = NewValue;
	}
	else
	{
		CurrentValue = FMath::Clamp(NewValue, MinValue, MaxValue);
	}
	UpdateDisplay();
}

void UBSWidget_IntSelector::SetRange(int32 InMin, int32 InMax)
{
	MinValue = InMin;
	MaxValue = InMax;
	CurrentValue = FMath::Clamp(CurrentValue, MinValue, MaxValue);
	UpdateDisplay();
}

void UBSWidget_IntSelector::SetDisplayTexts(const TArray<FText> InValueTextDisplays)
{
	ValueTextDisplays = InValueTextDisplays;
	UpdateDisplay();
}

void UBSWidget_IntSelector::Increment()
{
	SetValue(CurrentValue + 1);
	BroadcastValue();
}

void UBSWidget_IntSelector::Decrement()
{
	SetValue(CurrentValue - 1);
	BroadcastValue();
}

void UBSWidget_IntSelector::BroadcastValue()
{
	OnValueChanged.Broadcast(CurrentValue);
}

void UBSWidget_IntSelector::UpdateDisplay()
{
	if (TextValue)
	{
		TextValue->SetText(ValueTextDisplays.IsValidIndex(CurrentValue)
			                   ? ValueTextDisplays[CurrentValue]
			                   : FText::FromString("No Valid Texts"));
	}
}

