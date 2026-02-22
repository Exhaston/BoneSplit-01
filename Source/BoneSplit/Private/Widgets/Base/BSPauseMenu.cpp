// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Base/BSPauseMenu.h"

#include "CommonButtonBase.h"
#include "Actors/Player/BSLocalSaveSubsystem.h"
#include "Actors/Player/BSPlayerState.h"
#include "GameSystems/BSGameManagerSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
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
		if (const ABSPlayerState* PS = GetOwningPlayerState<ABSPlayerState>())
		{
			UBSLocalSaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<UBSLocalSaveSubsystem>();
			SaveSubsystem->SaveAscDataSync(GetOwningPlayer(), PS->GetAbilitySystemComponent());
			UBSGameManagerSubsystem::Get(GetOwningPlayer())->QuitToMenu();
		}
	});
	
	Button_Resume->OnClicked().AddWeakLambda(this, [this]()
	{
		DeactivateWidget();
	});
}

void UBSPauseMenu::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	if (ABSPlayerState* OwningPS = GetOwningPlayerState<ABSPlayerState>())
	{
		OwningPS->OnPlayerPaused();
	}
}

void UBSPauseMenu::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	if (ABSPlayerState* OwningPS = GetOwningPlayerState<ABSPlayerState>())
	{
		OwningPS->OnPlayerResumed();
	}
}
