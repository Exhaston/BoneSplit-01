// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BSFloatingNamePlate.generated.h"

class UBSAttributeBar;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSFloatingNamePlate : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void InitializeAbilitySystemComponent(UAbilitySystemComponent* InAsc);
	
protected:
	
	UPROPERTY(meta=(BindWidget))
	UBSAttributeBar* HealthBarWidget;
};
