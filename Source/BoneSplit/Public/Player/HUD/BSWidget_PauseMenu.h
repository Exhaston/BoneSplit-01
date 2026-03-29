// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BSWidget_PauseMenu.generated.h"

class UBSWidget_SettingsRoot;
class UCommonButtonBase;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWidget_PauseMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	//TODO: Session overview etc.
	
	virtual void NativeConstruct() override;
	
	virtual void NativeOnActivated() override;
	
	virtual void NativeOnDeactivated() override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWidget_SettingsRoot> SettingsRootClass;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* ResumeButton;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* SettingsButton;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* QuitButton;
};
