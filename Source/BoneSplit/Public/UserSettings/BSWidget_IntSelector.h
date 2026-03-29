// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "BSWidget_IntSelector.generated.h"

class UImage;
class UCommonRichTextBlock;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWidget_IntSelector : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnValueChanged, int32 NewValue);
	
	virtual void InitIntSelector(
		int32 InMinValue, int32 InMaxValue, TArray<FText> InValueTextDisplays, bool InCanWrap = true);

	//Sets the value without triggering on value changed callbacks.
	UFUNCTION(BlueprintCallable, Category = "Settings|Selector")
	void SetValue(int32 NewValue);

	UFUNCTION(BlueprintCallable, Category = "Settings|Selector")
	void SetRange(int32 InMin, int32 InMax);
	
	UFUNCTION(BlueprintCallable, Category = "Settings|Selector")
	int32 GetValue() const { return CurrentValue; }
	
	virtual void SetDisplayTexts(TArray<FText> InValueTextDisplays);
	
	FOnValueChanged& GetOnValueChangedDelegate() { return OnValueChanged; }

protected:
	
	FOnValueChanged OnValueChanged;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FText> ValueTextDisplays;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bCanWrap = true;
	
	virtual void NativeOnClicked() override;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
	
private:

	void Increment();
	void Decrement();
	void BroadcastValue();
	void UpdateDisplay();
	
	UPROPERTY(meta = (BindWidget))
	UImage* SelectionImage;
	
	UPROPERTY(meta = (BindWidget))
	UCommonRichTextBlock* TextValue;

	int32 CurrentValue = 0;
	int32 MinValue     = 0;
	int32 MaxValue     = 10;
};
