// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Base/BSPauseMenu.h"

#include "CommonButtonBase.h"
#include "GameSystems/BSGameManagerSubsystem.h"
#include "Player/BSPlayerStateBase.h"
#include "Widgets/BSLocalWidgetSubsystem.h"

UBSPauseMenu::UBSPauseMenu()
{
	bIsBackHandler = true;
}

void UBSPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Quit->OnClicked().AddWeakLambda(this, [this]()
	{
		UBSGameManagerSubsystem::Get(GetOwningPlayer())->QuitToMenu();
	});
	
	Button_Resume->OnClicked().AddWeakLambda(this, [this]()
	{
		DeactivateWidget();
	});
}

void UBSPauseMenu::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	if (ABSPlayerStateBase* OwningPS = GetOwningPlayerState<ABSPlayerStateBase>())
	{
		//OwningPS->OnPlayerPaused();
	}
}

void UBSPauseMenu::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	if (ABSPlayerStateBase* OwningPS = GetOwningPlayerState<ABSPlayerStateBase>())
	{
		//OwningPS->OnPlayerResumed();
	}
}
