// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/MainMenu/BSWMainMenu.h"

#include "GameSystems/BSGameManagerSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/BSLocalWidgetSubsystem.h"
#include "Widgets/Base/BSWUserConfirmContext.h"
#include "Widgets/Base/BSWButtonBase.h"

void UBSWMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	SinglePlayerButton->OnClicked().AddWeakLambda(this, [this]()
	{		
		UBSLocalWidgetSubsystem* LocalWidgetSubsystem = UBSLocalWidgetSubsystem::GetWidgetSubsystem(GetOwningPlayer());
		
		UBSWUserConfirmContext* ConfirmWindow = 
			CreateWidget<UBSWUserConfirmContext>(GetOwningPlayer(), LocalWidgetSubsystem->UserConfirmWidgetClass);
		
		ConfirmWindow->ConfirmButton->ButtonText = FText::FromString("Start Game!");
		ConfirmWindow->CancelButton->ButtonText = FText::FromString("Cancel");
		ConfirmWindow->ContextText->SetText(FText::FromString("Start Singleplayer Game?"));
		ConfirmWindow->OnConfirmed.AddWeakLambda(this, [this, LocalWidgetSubsystem]()
		{
			LocalWidgetSubsystem->ClearWidgets();
			UBSGameManagerSubsystem::Get(GetOwningPlayer())->StartLoadMap(HubLevel, "Spawn");
		});
		
		ConfirmWindow->AddToPlayerScreen();

	});
	QuitButton->OnClicked().AddWeakLambda(this, [this]()
	{
		const UBSLocalWidgetSubsystem* LocalWidgetSubsystem = UBSLocalWidgetSubsystem::GetWidgetSubsystem(GetOwningPlayer());
		
		UBSWUserConfirmContext* ConfirmWindow = 
			CreateWidget<UBSWUserConfirmContext>(GetOwningPlayer(), LocalWidgetSubsystem->UserConfirmWidgetClass);
		
		ConfirmWindow->ConfirmButton->ButtonText = FText::FromString("Quit");
		ConfirmWindow->CancelButton->ButtonText = FText::FromString("Cancel");
		ConfirmWindow->ContextText->SetText(FText::FromString("Quit Game?"));
		ConfirmWindow->OnConfirmed.AddWeakLambda(this, [this]()
		{
			UKismetSystemLibrary::QuitGame(
			GetOwningPlayer(), 
			GetOwningPlayer(),
			EQuitPreference::Quit,
			false);
		});
		
		ConfirmWindow->AddToPlayerScreen();
	});
}
