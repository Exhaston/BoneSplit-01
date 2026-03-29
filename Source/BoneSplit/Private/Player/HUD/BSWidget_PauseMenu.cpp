// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/HUD/BSWidget_PauseMenu.h"

#include "CommonButtonBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Match/BSOnlineSubsystem.h"
#include "Player/HUD/BSHudComponent.h"
#include "UserSettings/BSWidget_SettingsRoot.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UBSWidget_PauseMenu::NativeConstruct()
{
	Super::NativeConstruct();
	

}

void UBSWidget_PauseMenu::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	bIsBackHandler = true;
	
	ResumeButton->OnClicked().AddWeakLambda(this, [this]()
	{
		DeactivateWidget();
	});
	
	SettingsButton->OnClicked().AddWeakLambda(this, [this]()
	{
		UBSHudComponent::GetHudComponent(GetOwningPlayer())->GetFullscreenStack()->AddWidget(SettingsRootClass);
	});
	
	QuitButton->OnClicked().AddWeakLambda(this, [this]()
	{
		UBSOnlineSubsystem::Get(this)->LeaveGame();
	});
}

void UBSWidget_PauseMenu::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	QuitButton->OnClicked().Clear();
	SettingsButton->OnClicked().Clear();
	ResumeButton->OnClicked().Clear();
}
