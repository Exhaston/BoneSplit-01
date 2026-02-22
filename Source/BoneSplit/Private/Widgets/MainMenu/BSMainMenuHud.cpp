// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/MainMenu/BSMainMenuHud.h"
#include "Widgets/MainMenu/BSWMainMenu.h"
#include "GameInstance/BSLoadingScreenSubsystem.h"
#include "Widgets/BSLocalWidgetSubsystem.h"

void ABSMainMenuHud::NotifyReady()
{
	if (GetOwningPlayerController()->IsLocalController())
	{
		MainMenuRootInstance = CreateWidget<UBSWMainMenuRoot>(GetOwningPlayerController(), MainMenuRootWidgetClass);
		MainMenuRootInstance->AddToPlayerScreen();
		MainMenuRootInstance->GetWidgetStack()->AddWidget<UBSWMainMenu>(MainMenuWidgetClass);
		
		if (UBSLoadingScreenSubsystem* LoadingScreenSubsystem = GetGameInstance()->GetSubsystem<UBSLoadingScreenSubsystem>(); 
			LoadingScreenSubsystem && GetOwningPlayerController() && GetOwningPlayerController()->GetLocalPlayer())
		{
			LoadingScreenSubsystem->RemoveLoadingScreen(GetOwningPlayerController());
		}
	}
}

void ABSMainMenuHud::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MainMenuRootInstance)
	{
		MainMenuRootInstance->RemoveFromParent();
	}
	
	Super::EndPlay(EndPlayReason);
}
