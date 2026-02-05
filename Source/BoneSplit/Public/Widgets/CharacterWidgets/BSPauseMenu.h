// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BSPauseMenu.generated.h"

class UCommonButtonBase;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSPauseMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	UBSPauseMenu();
	
	virtual void NativeConstruct() override;
	
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* Button_Quit;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* Button_Settings;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* Button_Resume;
};
