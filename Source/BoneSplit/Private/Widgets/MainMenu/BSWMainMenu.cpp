// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/MainMenu/BSWMainMenu.h"
#include "Widgets/MainMenu/BSWServerBrowser.h"
#include "GameSystems/BSGameManagerSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/BSLocalWidgetSubsystem.h"
#include "Widgets/Base/BSWUserConfirmContext.h"
#include "Widgets/Base/BSWButtonBase.h"
#include "Widgets/MainMenu/BSMainMenuHud.h"

void UBSWMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	MultiPlayerButton->OnClicked().AddWeakLambda(this, [this]()
	{
		ABSMainMenuHud* MainMenuHud = GetOwningPlayer()->GetHUD<ABSMainMenuHud>();
		MainMenuHud->GetWidgetStack()->AddWidget(MainMenuHud->ServerBrowserClass);
	});
	SinglePlayerButton->OnClicked().AddWeakLambda(this, [this]()
	{
		const ABSMainMenuHud* MainMenuHud = GetOwningPlayer()->GetHUD<ABSMainMenuHud>();
		
		UBSWUserConfirmContext* ConfirmWindow = 
			CreateWidget<UBSWUserConfirmContext>(GetOwningPlayer(), MainMenuHud->ConfirmContextWidgetClass);
		
		ConfirmWindow->InitializeUserConfirmWidget(
			FText::FromString("Start Singleplayer Game?"), 
			FText::FromString("Start Game!"), 
			FText::FromString("Cancel"), 
			true);
		
		ConfirmWindow->OnConfirmed.AddWeakLambda(this, [this]()
		{
			UBSGameManagerSubsystem::Get(GetOwningPlayer())->InitializeHost();
		});
		
		ConfirmWindow->AddToPlayerScreen();

	});
	QuitButton->OnClicked().AddWeakLambda(this, [this]()
	{
		const ABSMainMenuHud* MainMenuHud = GetOwningPlayer()->GetHUD<ABSMainMenuHud>();
		
		UBSWUserConfirmContext* ConfirmWindow = 
			CreateWidget<UBSWUserConfirmContext>(GetOwningPlayer(), MainMenuHud->ConfirmContextWidgetClass);
		
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
