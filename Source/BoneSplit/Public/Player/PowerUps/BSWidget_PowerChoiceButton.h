// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "BSWidget_PowerChoiceButton.generated.h"

enum EBSPowerUpRarity : int;
class UCommonRichTextBlock;
class UCommonLazyImage;
class UTextBlock;
class UImage;
class UBSPowerUpData;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWidget_PowerChoiceButton : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:

	void InitializeButton(UBSPowerUpData* InPowerUpData);
	
	UFUNCTION(BlueprintNativeEvent)
	void PowerUpRaritySet(EBSPowerUpRarity OutRarity);
	
protected:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonLazyImage> PowerUpIcon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonRichTextBlock> PowerUpName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonRichTextBlock> PowerUpInfo;

private:

	UPROPERTY()
	TObjectPtr<UBSPowerUpData> PowerUpData;
};
