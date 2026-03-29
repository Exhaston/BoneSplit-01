// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BSWMainMenu.generated.h"

class UCommonButtonBase;
class UBSWidget_SettingsRoot;
class UCommonRichTextBlock;
class UBSWButtonBase;
/**
 * 
 */
UCLASS(DisplayName="BSMainMenuWidgetBase")
class BONESPLIT_API UBSWMainMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* SinglePlayerButton;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* MultiPlayerButton;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* SettingsButton;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWidget_SettingsRoot> SettingsRootClass;
	
	UPROPERTY(meta=(BindWidget))
	UCommonButtonBase* QuitButton;
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* VersionText;
	
};
