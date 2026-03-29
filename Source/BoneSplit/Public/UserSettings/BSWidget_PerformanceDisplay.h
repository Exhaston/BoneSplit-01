// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BSWidget_PerformanceDisplay.generated.h"

class UCommonRichTextBlock;

namespace BSConsoleVariables
{
	extern TAutoConsoleVariable<bool> CVarBSEnablePerf;
}
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWidget_PerformanceDisplay : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual void UpdateFPS(float DeltaSeconds);
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* FPSCounter;
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* LatencyCounter;
	
private:
	
	static constexpr int32 FPS_Sample_Count = 60;
	float FrameTimeSamples[FPS_Sample_Count] = {};
	int32 SampleIndex = 0;
	int32 SamplesFilled = 0;
	
	float AccumulatedTime = 0.f;
	static constexpr float UpdateInterval = 0.5f;
};
