// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSWMainMenuRoot.h"
#include "GameFramework/HUD.h"
#include "BSMainMenuHud.generated.h"

class UBSWUserConfirmContext;
class UBSWServerButton;
class UBSWServerBrowser;
class UBSWMainMenu;
class UBSWRoot;
class UBSWMainMenuRoot;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API ABSMainMenuHud : public AHUD
{
	GENERATED_BODY()
	
public:
	
	virtual void NotifyReady();
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UCommonActivatableWidgetStack* GetWidgetStack() { return MainMenuRootInstance->GetWidgetStack(); }
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWMainMenuRoot> MainMenuRootWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWMainMenu> MainMenuWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWServerBrowser> ServerBrowserClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWServerButton> ServerBrowserButtonClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWUserConfirmContext> ConfirmContextWidgetClass;

protected:
	
	UPROPERTY()
	UBSWMainMenuRoot* MainMenuRootInstance;
};
