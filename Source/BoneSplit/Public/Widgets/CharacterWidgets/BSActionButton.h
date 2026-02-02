// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "BoneSplit/BoneSplit.h"
#include "BSActionButton.generated.h"

class UCommonLazyImage;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSActionButton : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag AbilityTag = BSTags::Ability_Player_PrimaryArm;
	
	UPROPERTY(meta=(BindWidget))
	UCommonLazyImage* AbilityIcon;
	
	UPROPERTY(meta=(BindWidget))
	UCommonLazyImage* BackgroundImage;
	
};
