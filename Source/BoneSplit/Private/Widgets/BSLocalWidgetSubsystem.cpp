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
	
	if (!DS->GameRootWidgetClass.IsNull())
	{
		GameRootWidgetClass = DS->GameRootWidgetClass.LoadSynchronous();
	}
	if (!DS->GameRootWidgetClass.IsNull())
	{
		MainMenuWidgetRootClass = DS->MainMenuRootWidgetClass.LoadSynchronous();
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
	if (!DS->DefaultDamageNumberWidgetClass.IsNull())
	{
		DamageNumberWidgetClass = DS->DefaultDamageNumberWidgetClass.LoadSynchronous();
	}
	
	if (!DS->UserConfirmContextWidgetClass.IsNull())
	{
		UserConfirmWidgetClass = DS->UserConfirmContextWidgetClass.LoadSynchronous();
	}
}

void UBSLocalWidgetSubsystem::ClearWidgets()
{
	if (RootWidgetInstance)
	{
		RootWidgetInstance->RemoveFromParent();
		RootWidgetInstance = nullptr;
	}
}

UBSWToolTipBase* UBSLocalWidgetSubsystem::CreateGenericToolTip(
	const FText& Header, const FText& Text, const FText& AltHeader)
{
	UBSWToolTipBase* ToolTip = CreateWidgetFromClass(DefaultToolTipWidgetClass);
	ToolTip->SetToolTipText(Header, Text, AltHeader);
	return ToolTip;
}

UBSWRoot* UBSLocalWidgetSubsystem::CreatePlayerUI(APlayerController* InPlayerController, bool bMainMenu)
{
	check(InPlayerController);
	check(GameRootWidgetClass);
	RootWidgetInstance = CreateWidget<UBSWRoot>(InPlayerController, bMainMenu ? MainMenuWidgetRootClass : GameRootWidgetClass, "RootWidget");
	RootWidgetInstance->AddToPlayerScreen();
	return RootWidgetInstance;
}

void UBSLocalWidgetSubsystem::SpawnDamageNumber(const FGameplayEventData EventData)
{
	//TODO; This will be called A LOT. Maybe cache some values here to reduce load.
	if (!GetRootWidgetInstance()) return;
	if (!GetRootWidgetInstance()->GetOwningPlayerPawn()) return;
	
	bool bBlocked = false;
	
	if (GetRootWidgetInstance()->GetOwningPlayerPawn() == EventData.Target)
	{
		if (!BSConsoleVariables::CVarBSShowIncomingDamageNumbers.GetValueOnGameThread()) bBlocked = true;
	}
	else
	{
		if (!BSConsoleVariables::CVarBSShowDamageNumbers.GetValueOnGameThread()) bBlocked = true;
	}
	
	if (bBlocked) return;
	
	UBSWDamageNumber* NewDamageNumber = CreateWidgetFromClass<UBSWDamageNumber>(DamageNumberWidgetClass);
	NewDamageNumber->InitializeDamageNumber(EventData);
	NewDamageNumber->AddToPlayerScreen();
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

UBSWRoot* UBSLocalWidgetSubsystem::GetRootWidgetInstance()
{
	return RootWidgetInstance;
}
