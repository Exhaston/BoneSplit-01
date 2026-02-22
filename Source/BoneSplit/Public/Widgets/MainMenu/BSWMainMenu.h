// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BSWMainMenu.generated.h"

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
	
	UPROPERTY(meta=(BindWidgetOptional))
	UBSWButtonBase* ReconnectButton;
	
	UPROPERTY(meta=(BindWidget))
	UBSWButtonBase* SinglePlayerButton;
	
	UPROPERTY(meta=(BindWidget))
	UBSWButtonBase* MultiPlayerButton;
	
	UPROPERTY(meta=(BindWidget))
	UBSWButtonBase* SettingsButton;
	
	UPROPERTY(meta=(BindWidget))
	UBSWButtonBase* QuitButton;
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* VersionText;
	
};
