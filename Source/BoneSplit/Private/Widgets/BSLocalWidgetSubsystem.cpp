// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/BSLocalWidgetSubsystem.h"

#include "GameSettings/BSDeveloperSettings.h"

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
}

UBSWRoot* UBSLocalWidgetSubsystem::CreatePlayerUI(APlayerController* InPlayerController)
{
	check(InPlayerController);
	check(RootWidgetClass);
	RootWidgetInstance = CreateWidget<UBSWRoot>(InPlayerController, RootWidgetClass, "RootWidget");
	RootWidgetInstance->AddToPlayerScreen();
	return RootWidgetInstance;
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
