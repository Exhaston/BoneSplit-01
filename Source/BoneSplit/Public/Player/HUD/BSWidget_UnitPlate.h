// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSWidget_UnitPlate.generated.h"

class UCommonRichTextBlock;
class UAbilitySystemComponent;
class UBSWidget_AttributeBar;
/**
 * 
 */
UCLASS(Abstract)
class BONESPLIT_API UBSWidget_UnitPlate : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeOnInitialized() override;
	
	virtual void NativeConstruct() override;
	
	virtual void SetOwningAsc(UAbilitySystemComponent* InAsc);
	virtual void SetUnitName(const FText& InName);
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* NameText;
	UPROPERTY(meta=(BindWidget))
	UBSWidget_AttributeBar* HealthBar;
	
};
