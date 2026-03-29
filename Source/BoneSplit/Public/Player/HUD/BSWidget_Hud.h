// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BSWidget_Hud.generated.h"

class UBSWidget_PlayerFrame;
class UBSWidget_AttributeBar;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWidget_Hud : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_PlayerFrame* PlayerFrameWidget;
	
	UPROPERTY(meta=(BindWidget))
	UBSWidget_AttributeBar* SoulBar;
};
