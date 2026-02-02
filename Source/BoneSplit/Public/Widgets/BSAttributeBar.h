// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "BSAttributeBar.generated.h"

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
	
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void UpdateBar(float DeltaTime);
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float InterpSpeed = 8;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayAttribute CurrentAttribute = UBSAttributeSet::GetHealthAttribute();
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayAttribute MaxAttribute = UBSAttributeSet::GetMaxHealthAttribute();
	
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bPlayerAsc = false;
	
	UPROPERTY()
	float SmoothedCurrent = 1;
	UPROPERTY()
	float Current = 1;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(BindWidget=true))
	UImage* HealthBarImage;
};
