// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "MainMenu/BSWidget_MainMenuRoot.h"

#include "CommonButtonBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Match/BSOnlineSettings.h"

void UBSWidget_MainMenuRoot::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Singleplayer->OnClicked().AddWeakLambda(this, [this]()
	{
		if (const TSoftObjectPtr<UWorld> LobbyMap = GetBSOnlineSettings()->LobbyMap; !LobbyMap.IsNull())
		{
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, LobbyMap, true);
		}
	});
	
	Button_Multiplayer->OnClicked().AddWeakLambda(this, [this]()
	{
		if (const TSoftObjectPtr<UWorld> LobbyMap = GetBSOnlineSettings()->LobbyMap; !LobbyMap.IsNull())
		{
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, LobbyMap, true, "?Listen");
		}
	});

	
	Button_Settings->OnClicked().AddWeakLambda(this, [this]()
	{
		
	});
	
	Button_Quit->OnClicked().AddWeakLambda(this, [this]()
	{
		UKismetSystemLibrary::QuitGame(
			GetOwningPlayer(), 
			GetOwningPlayer(),
			EQuitPreference::Quit,
			false);
	});
}

void UBSWidget_MainMenuRoot::NativeDestruct()
{
	Super::NativeDestruct();
	
	Button_Singleplayer->OnClicked().Clear();
	Button_Multiplayer->OnClicked().Clear();
	Button_Settings->OnClicked().Clear();
	Button_Quit->OnClicked().Clear();
}

UCommonActivatableWidgetSwitcher* UBSWidget_MainMenuRoot::GetMainSwitcher()
{
	return Switcher_MainTab;
}
