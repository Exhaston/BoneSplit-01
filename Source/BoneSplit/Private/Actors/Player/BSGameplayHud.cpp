// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSGameplayHud.h"
#include "BoneSplit/BoneSplit.h"
#include "Widgets/Base/BSPauseMenu.h"
#include "Widgets/BSLocalWidgetSubsystem.h"
#include "Widgets/HUD/BSGameplayRootWidget.h"
#include "Widgets/HUD/BSCharacterPane.h"
#include "Widgets/HUD/BSWHud.h"

void ABSGameplayHud::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GameplayRootWidgetInstance)
	{
		GameplayRootWidgetInstance->RemoveFromParent();
	}
	
	if (!GetOwningPlayerController() || !GetOwningPlayerController()->IsLocalController()) return;
	
	if (GetGameInstance())
	{
		/*
		if (UBSLoadingScreenSubsystem* LoadingScreenSubsystem = GetGameInstance()->GetSubsystem<UBSLoadingScreenSubsystem>(); 
		LoadingScreenSubsystem && GetOwningPlayerController() && GetOwningPlayerController()->GetLocalPlayer())
		{
			LoadingScreenSubsystem->AddLoadingScreen(GetOwningPlayerController());
		}
		*/
	}
	
	Super::EndPlay(EndPlayReason);
}

void ABSGameplayHud::SpawnWidgets()
{
	if (GetOwningPlayerController()->IsLocalController())
	{
		GameplayRootWidgetInstance = CreateWidget<UBSGameplayRootWidget>(GetOwningPlayerController(), GameplayRootWidgetClass);
		GameplayRootWidgetInstance->AddToPlayerScreen();
		GameplayRootWidgetInstance->WidgetStack->AddWidget(HudWidgetClass);
		
		/*
		if (GetOwningPlayerController()->IsLocalController())
		{
			UBSLoadingScreenSubsystem* LoadingScreenSubsystem = 
				GetGameInstance()->GetSubsystem<UBSLoadingScreenSubsystem>();
			
			LoadingScreenSubsystem->RemoveLoadingScreen(GetOwningPlayerController());
		}
		*/
	}
}

void ABSGameplayHud::SpawnDamageNumber(const FGameplayEventData EventData)
{
	//TODO; This will be called A LOT. Maybe cache some values here to reduce load.
	if (!GetOwningPawn() || !EventData.Target) return;
	
	bool bBlocked = false;
	
	if (GetOwningPawn() == EventData.Target)
	{
		if (!BSConsoleVariables::CVarBSShowIncomingDamageNumbers.GetValueOnGameThread()) bBlocked = true;
	}
	else
	{
		if (!BSConsoleVariables::CVarBSShowDamageNumbers.GetValueOnGameThread()) bBlocked = true;
	}
	
	if (bBlocked) return;
	
	UBSWDamageNumber* NewDamageNumber = CreateWidget<UBSWDamageNumber>(GetOwningPlayerController(), FloatingDamageNumberClass);
	NewDamageNumber->AddToPlayerScreen();
	NewDamageNumber->InitializeDamageNumber(EventData);
}

bool ABSGameplayHud::IsInPauseMenu()
{
	if (GetOwningPlayerController() && GetOwningPlayerController()->IsLocalController())
	{
		if (GameplayRootWidgetInstance && PauseMenuClass)
		{
			return GameplayRootWidgetInstance->WidgetStack->GetWidgetList().ContainsByPredicate(
				[this](const UCommonActivatableWidget* Widget)
			{
				return Widget && Widget->GetClass() == PauseMenuClass;
			});
		}
	}
	return false;
}

void ABSGameplayHud::TogglePauseWidget()
{
	if (GetOwningPlayerController() && GetOwningPlayerController()->IsLocalController())
	{
		if (!IsInPauseMenu() && GameplayRootWidgetInstance && PauseMenuClass)
		{
			GameplayRootWidgetInstance->WidgetStack->AddWidget(PauseMenuClass);
		}
	}
}

void ABSGameplayHud::ToggleCharacterPaneWidget()
{
	if (GetOwningPlayerController() && GetOwningPlayerController()->IsLocalController())
	{
		if (GameplayRootWidgetInstance && CharacterPaneClass)
		{
			if (auto* FoundWidget = 
				GameplayRootWidgetInstance->WidgetStack->GetWidgetList().FindByPredicate(
				[this](const UCommonActivatableWidget* Widget)
				{
					return Widget && Widget->GetClass() == CharacterPaneClass;
				}))
			{
				if (*FoundWidget)
				{
					(*FoundWidget)->DeactivateWidget();
				}
			}
			else if (!IsInPauseMenu())
			{
				GameplayRootWidgetInstance->WidgetStack->AddWidget(CharacterPaneClass);
			}
		}
	}
}
