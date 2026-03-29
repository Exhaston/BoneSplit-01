// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BSWidget_MainMenuRoot.generated.h"

class UCommonButtonBase;
class UCommonActivatableWidgetSwitcher;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWidget_MainMenuRoot : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	UCommonActivatableWidgetSwitcher* GetMainSwitcher();
	
protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UCommonButtonBase* Button_Singleplayer;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UCommonButtonBase* Button_Multiplayer;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UCommonButtonBase* Button_Settings;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UCommonButtonBase* Button_Quit;
	
	UPROPERTY(meta=(BindWidget))
	UCommonActivatableWidgetSwitcher* Switcher_MainTab;
};
