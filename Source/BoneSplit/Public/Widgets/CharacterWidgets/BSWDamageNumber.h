// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonRichTextBlock.h"
#include "CommonUserWidget.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BSWDamageNumber.generated.h"

struct FGameplayAttribute;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWDamageNumber : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void InitializeDamageNumber(FGameplayEventData EventData);
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual void SetLocation();
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* FloatAnimation;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ImpactAnimation;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* CritAnimation;
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* RichDamageText;
	
	UPROPERTY()
	FVector2D RandomScreenOffset;
	
	UPROPERTY()
	FVector WorldLocation;
};
