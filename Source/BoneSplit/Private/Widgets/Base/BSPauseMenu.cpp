// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Base/BSPauseMenu.h"

#include "CommonButtonBase.h"
#include "Actors/Player/BSLocalSaveSubsystem.h"
#include "Actors/Player/BSPlayerState.h"
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
			UKismetSystemLibrary::QuitGame(
				PS, 
				PS->GetPlayerController(), 
				EQuitPreference::Quit, 
				false);
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
	
	if (GetOwningLocalPlayer())
	{
		UBSLocalWidgetSubsystem* Subsystem = GetOwningLocalPlayer()->GetSubsystem<UBSLocalWidgetSubsystem>();
		Subsystem->bIsPaused = true;
		Subsystem->GetOnPauseMenuDelegate().Broadcast();
	}
}

void UBSPauseMenu::NativeOnDeactivated()
{
	if (GetOwningLocalPlayer())
	{
		UBSLocalWidgetSubsystem* Subsystem = GetOwningLocalPlayer()->GetSubsystem<UBSLocalWidgetSubsystem>();

		if (IsValid(Subsystem) && Subsystem->GetOnPauseMenuDelegate().IsBound())
		{
			Subsystem->bIsPaused = false;
			Subsystem->GetOnResumeDelegate().Broadcast();
		}
	}
	
	Super::NativeOnDeactivated();
}
