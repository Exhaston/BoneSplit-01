// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonLazyImage.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "BSWEquipmentMinimalDisplay.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWEquipmentMinimalDisplay : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag EquipmentSlotToTrack;
	
	UPROPERTY(meta=(BindWidget))
	UCommonLazyImage* EquipmentIconImage;
};
