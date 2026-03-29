// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "UserSettings/BSWidget_PerformanceDisplay.h"

#include "CommonRichTextBlock.h"
#include "GameFramework/PlayerState.h"

namespace BSConsoleVariables
{
	TAutoConsoleVariable<bool> CVarBSEnablePerf(
		TEXT("BS.Player.EnablePerf"),
		false,
		TEXT("0 = Default"),
		ECVF_Default);
}

void UBSWidget_PerformanceDisplay::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibilityInternal(BSConsoleVariables::CVarBSEnablePerf.GetValueOnGameThread() ? 
		ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	
	BSConsoleVariables::CVarBSEnablePerf->OnChangedDelegate().AddWeakLambda(
	this, [this](IConsoleVariable* ConsoleVariable)
	{
		SetVisibilityInternal(ConsoleVariable->GetBool() ? 
			ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);	
	});
}

void UBSWidget_PerformanceDisplay::NativeDestruct()
{
	Super::NativeDestruct();
	BSConsoleVariables::CVarBSEnablePerf->OnChangedDelegate().RemoveAll(this);
}

void UBSWidget_PerformanceDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!GetOwningPlayer() || !GetOwningPlayer()->GetPlayerState<APlayerState>()) return;
	if (!GEngine) return;
	
	// Always sample every frame for accuracy
	FrameTimeSamples[SampleIndex] = InDeltaTime;
	SampleIndex = (SampleIndex + 1) % FPS_Sample_Count;
	SamplesFilled = FMath::Min(SamplesFilled + 1, FPS_Sample_Count);
	
	AccumulatedTime += InDeltaTime;
	if (AccumulatedTime < UpdateInterval)
	{
		return;
	}
	AccumulatedTime = 0.f;
	
	UpdateFPS(InDeltaTime);
	
	const float PingMS = GetOwningPlayer()->GetPlayerState<APlayerState>()->ExactPing;
	LatencyCounter->SetText(FText::FromString("MS: " + FString::FromInt(PingMS)));
}

void UBSWidget_PerformanceDisplay::UpdateFPS(float DeltaSeconds)
{
	// Average over however many samples we have so far
	float Sum = 0.f;
	for (int32 i = 0; i < SamplesFilled; ++i)
	{
		Sum += FrameTimeSamples[i];
	}
	const float AvgFPS = static_cast<float>(SamplesFilled) / Sum;

	FPSCounter->SetText(FText::FromString(
		FString::Printf(TEXT("FPS: %.0f"), AvgFPS)
	));
}
