// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Blueprint/UserWidget.h"
#include "BSWidget_FloatingText.generated.h"

class UCommonTextStyle;
class UCommonRichTextBlock;

namespace BSGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Widget_FloatingText_Default);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Widget_FloatingText_Helpful);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Widget_FloatingText_Crit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Widget_FloatingText_Impact);
}

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DisplayName="Floating Text")
class BONESPLIT_API UBSWidget_FloatingText : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	/* Current tags that define style:
	 * Widget_FloatingText_Crit
	 * Widget_FloatingText_Default
	 * Widget_FloatingText_Helpful
	 * Widget_FloatingText_Impact
	 */
	virtual void SetFloatingText(FVector InWorldLocation, const FText& Text, const FGameplayTagContainer& RelevantTags);
	
protected:
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void ProjectToScreen();
	
	virtual void SetStyle(const FGameplayTagContainer& RelevantTags);
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* RichDamageText;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* FloatAnimation;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ImpactAnimation;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* CritAnimation;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UCommonTextStyle> PhysicalDamageStyle;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UCommonTextStyle> MagicDamageStyle;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UCommonTextStyle> HelpfulStyle;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UCommonTextStyle> ImpactStyle;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UCommonTextStyle> CriticalStyle;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector2D RandomOffsetBounds = FVector2D::ZeroVector;
	
	FVector2D RandomScreenOffset = FVector2D::ZeroVector;
	
	FVector WorldLocation;
};
