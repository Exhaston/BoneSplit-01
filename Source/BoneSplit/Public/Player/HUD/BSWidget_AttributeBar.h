// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedProgressBarWidget.h"
#include "AttributeSet.h"
#include "BSWidget_AttributeBar.generated.h"

struct FOnAttributeChangeData;
class UAbilitySystemComponent;
struct FGameplayAttribute;
class UCommonRichTextBlock;

namespace BSConsoleVariables
{
	extern TAutoConsoleVariable<bool> CVarBSBarsShowNumbers;
	extern TAutoConsoleVariable<bool> CVarBSBarsShowPercentages;
}
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWidget_AttributeBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void SetAbilitySystemToTrack(
		FGameplayAttribute InCurrentAttribute, FGameplayAttribute InMaxAttribute, UAbilitySystemComponent* InAsc);
	
	virtual void OnAttributeValueChanged(const FOnAttributeChangeData& NewData);
	
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void SetProgress(float NewProgress);
	virtual void SetBonusProgress(float NewBonusProgress);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	UMaterialInstanceDynamic* GetBarMaterial();

	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* ProgressBarValueText;
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* ProgressBarPercentText;
	
	UPROPERTY(meta=(BindWidget))
	UImage* ProgressBarImage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FLinearColor FillColor = FLinearColor::Red;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FLinearColor DeltaColor = FLinearColor::White;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FLinearColor BonusColor = FLinearColor::Green;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float InterpSpeed = 8;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> TrackedAsc = nullptr;
	
	FGameplayAttribute CurrentAttribute;
	FGameplayAttribute MaxAttribute;
	
	float Progress = 0.f;
	float BonusProgress = 0.f;
	float OldProgress = 0.f;
};
