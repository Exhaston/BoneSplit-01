// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "BSAttributeBar.generated.h"

class UCommonTextBlock;
class UBSAttributeSet;
struct FGameplayAttribute;
class UImage;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BONESPLIT_API UBSAttributeBar : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;
	
	virtual UMaterialInstanceDynamic* GetBarMaterial();
	
	virtual void InitializeAttributeBar(UAbilitySystemComponent* Asc);
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void SmoothOldToCurrent(float DeltaTime);
	
	virtual void SetAttributeValues(float InCurrentValue, float InMaxValue);
	virtual void SetTextElements(int32 InCurrentValue, int32 InMaxValue, float InNormalizedPercent);
	virtual void SetBarProgress(float InNormalizedPercent);
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float InterpSpeed = 8;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FLinearColor Color = FLinearColor::Red;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayAttribute CurrentAttribute = UBSAttributeSet::GetHealthAttribute();
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayAttribute MaxAttribute = UBSAttributeSet::GetMaxHealthAttribute();
	
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	float SmoothNormalizedPercent = 1;
	
	UPROPERTY()
	float NormalizedPercent = 1;
	
	UPROPERTY()
	float Value = 100;
	
	UPROPERTY()
	float MaxValue = 100;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(BindWidget=true))
	UImage* HealthBarImage;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(BindWidget=true))
	UCommonTextBlock* HealthBarText;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(BindWidget=true))
	UCommonTextBlock* HealthBarPercentText;
};
