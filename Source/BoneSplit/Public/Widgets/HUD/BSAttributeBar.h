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
 * @brief Attribute bar that utilizes an ability 
 * system to visualize the current values of attributes related to their max value.
 *
 * These can live on components and HUD alike. Widget components' widgets are still the viewing player.
 * Therefore, we need to set owner of the attribute manually during initialization or construction.
 *
 * @note Will not function if ability system is not set manually. 
 * Marked as abstract and expects a widget designed in blueprint with a material.
 * 
 * @see SetAttributeOwner
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BONESPLIT_API UBSAttributeBar : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;
	
	virtual void RemoveFromParent() override;
	
	//Initializes this attribute bar with the owner. Binds delegates when attribute values changes on the asc.
	virtual void SetAttributeOwner(UAbilitySystemComponent* Asc);
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	
	virtual UMaterialInstanceDynamic* GetBarMaterial();
	
	virtual void SetAttributeValues(float InCurrentValue, float InMaxValue);
	
	void SmoothOldToCurrent(float DeltaTime);
	
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
	
	//Expected name of the current normalized percentage 0 - 1 within the material, will be set when the bar updates.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FName HealthBarColorParamName = "Color";
	
	//Expected name of the current normalized percentage 0 - 1 within the material, will be set when the bar updates.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FName HealthBarPercentParamName = "Current";
	
	//Expected name of the Old normalized percentage 0 - 1 within the material, will be set when the bar updates.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FName HealthBarOldPercentParamName = "Previous";
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(BindWidget=true))
	UCommonTextBlock* HealthBarText;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(BindWidget=true))
	UCommonTextBlock* HealthBarPercentText;
};
