// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "BSWidget_PlayerFrame.generated.h"

class UCommonRichTextBlock;
class UBSWidget_AttributeBar;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWidget_PlayerFrame : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void SetPlayerAbilitySystem(UAbilitySystemComponent* InAbilitySystem, FString PlayerName);
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* PlayerNameText;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_AttributeBar* HealthBar;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_AttributeBar* ManaBar;
};
