// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSCharacterPane.h"

#include "Widgets/BSLocalWidgetSubsystem.h"

void UBSCharacterPane::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	if (GetOwningLocalPlayer())
	{
		UBSLocalWidgetSubsystem* Subsystem = GetOwningLocalPlayer()->GetSubsystem<UBSLocalWidgetSubsystem>();
		Subsystem->GetOnCharacterPaneDelegate().Broadcast();
	}
}

void UBSCharacterPane::NativeOnDeactivated()
{
	if (GetOwningLocalPlayer())
	{
		UBSLocalWidgetSubsystem* Subsystem = GetOwningLocalPlayer()->GetSubsystem<UBSLocalWidgetSubsystem>();

		if (IsValid(Subsystem) && Subsystem->GetOnPauseMenuDelegate().IsBound())
		{
			Subsystem->GetOnCharacterPaneCloseDelegate().Broadcast();
		}
	}
	
	Super::NativeOnDeactivated();
}
