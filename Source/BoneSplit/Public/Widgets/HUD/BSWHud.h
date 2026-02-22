// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CommonActivatableWidget.h"
#include "BSWHud.generated.h"

class UBSActionButton;
class UBSAttributeBar;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWHud : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

protected:
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSAttributeBar> HealthBar;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSAttributeBar> ManaBar;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSAttributeBar> SoulBar;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSActionButton> ActionButton1;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSActionButton> ActionButton2;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSActionButton> ActionButton3;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSActionButton> ActionButton4;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSActionButton> ActionButton5;
	
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
};
