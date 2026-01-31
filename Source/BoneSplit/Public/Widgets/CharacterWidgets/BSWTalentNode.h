// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "BSWTalentNode.generated.h"

class ABSPlayerState;
class UCommonTextBlock;
class UCommonNumericTextBlock;
class UCommonLazyImage;
class UBSTalentEffect;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWTalentNode : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;
	
	virtual void RemoveFromParent() override;
	
	virtual void UpdateTalentWidget();
	
	virtual void UpdateColor(ABSPlayerState* PS, int32 Level);
	virtual void UpdateText(int32 Level);
	
	virtual void NativeOnPressed() override;
	
	UPROPERTY(meta=(BindWidget))
	UCommonTextBlock* LevelText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonLazyImage> TalentIconImage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSTalentEffect> TalentData;
};
