// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BSWTalentTree.generated.h"

class UBSTalentEffect;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWTalentTree : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(meta=(BindWidget))
	UPanelWidget* TreeParentWidget;
	
};
