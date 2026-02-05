// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/BSLocalWidgetSubsystem.h"

#include "GameSettings/BSDeveloperSettings.h"

UBSLocalWidgetSubsystem* UBSLocalWidgetSubsystem::GetWidgetSubsystem(const UUserWidget* Context)
{
	return Context->GetOwningLocalPlayer()->GetSubsystem<UBSLocalWidgetSubsystem>();
}

UBSLocalWidgetSubsystem* UBSLocalWidgetSubsystem::GetWidgetSubsystem(const APlayerController* Context)
{
	check(Context->GetLocalPlayer());
	return Context->GetLocalPlayer()->GetSubsystem<UBSLocalWidgetSubsystem>();
}

UBSLocalWidgetSubsystem* UBSLocalWidgetSubsystem::GetWidgetSubsystem(const ULocalPlayer* Context)
{
	return Context->GetSubsystem<UBSLocalWidgetSubsystem>();
}

void UBSLocalWidgetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	const UBSDeveloperSettings* DS = GetDefault<UBSDeveloperSettings>();
	
	if (!DS->RootWidgetClass.IsNull())
	{
		RootWidgetClass = DS->RootWidgetClass.LoadSynchronous();
	}
	
	if (!DS->HudWidgetClass.IsNull())
	{
		HudWidgetClass = DS->HudWidgetClass.LoadSynchronous();
	}
	
	if (!DS->PauseMenuWidgetClass.IsNull())
	{
		PauseMenuWidgetClass = DS->PauseMenuWidgetClass.LoadSynchronous();
	}
	
	if (!DS->CharacterPaneWidgetClass.IsNull())
	{
		CharacterPaneWidgetClass = DS->CharacterPaneWidgetClass.LoadSynchronous();
	}
	
	if (!DS->DefaultTooltipWidgetClass.IsNull())
	{
		DefaultToolTipWidgetClass = DS->DefaultTooltipWidgetClass.LoadSynchronous();
	}
}

UBSWToolTipBase* UBSLocalWidgetSubsystem::CreateGenericToolTip(
	const FText& Header, const FText& Text, const FText& AltHeader)
{
	UBSWToolTipBase* ToolTip = CreateWidgetFromClass(DefaultToolTipWidgetClass);
	ToolTip->SetToolTipText(Header, Text, AltHeader);
	return ToolTip;
}

UBSWRoot* UBSLocalWidgetSubsystem::CreatePlayerUI(APlayerController* InPlayerController)
{
	check(InPlayerController);
	check(RootWidgetClass);
	RootWidgetInstance = CreateWidget<UBSWRoot>(InPlayerController, RootWidgetClass, "RootWidget");
	RootWidgetInstance->AddToPlayerScreen();
	return RootWidgetInstance;
}

bool UBSLocalWidgetSubsystem::IsWidgetActive(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	check(WidgetClass);
	check(RootWidgetInstance);
	return RootWidgetInstance->WidgetStack->GetWidgetList().ContainsByPredicate(
	[WidgetClass](const UCommonActivatableWidget* Widget)
	{
		return Widget->IsA(WidgetClass);
	}); 
}

void UBSLocalWidgetSubsystem::RemoveWidgetFromStack(const TSubclassOf<UCommonActivatableWidget> WidgetInstance) const
{
	check(RootWidgetInstance);
	for (const auto Widget : RootWidgetInstance->WidgetStack->GetWidgetList())
	{
		if (Widget->IsA(WidgetInstance)) Widget->DeactivateWidget();
		break;
	}
}

FBSWidgetDelegate& UBSLocalWidgetSubsystem::GetOnPauseMenuDelegate()
{
	return OnPauseMenuDelegate;
}

FBSWidgetDelegate& UBSLocalWidgetSubsystem::GetOnResumeDelegate()
{
	return OnResumeDelegate;
}

UBSWRoot* UBSLocalWidgetSubsystem::GetRootWidgetInstance()
{
	return RootWidgetInstance;
}
