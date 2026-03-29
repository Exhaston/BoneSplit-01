// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "MainMenu/BSMenuPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "BoneSplit/BoneSplit.h"
#include "MainMenu/BSWidget_MainMenuRoot.h"

void ABSMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!MainMenuRootClass)
	{
		UE_LOG(BoneSplit, Error, TEXT("No Main Menu Root class found on the menu controller. Please assign one."));  
		return;
	}
	
	if (!MainMenuRootInstance && IsLocalController() && IsPrimaryPlayer())
	{
		MainMenuRootInstance = CreateWidget<UBSWidget_MainMenuRoot>(this);
		MainMenuRootInstance->AddToPlayerScreen();
	}
}
