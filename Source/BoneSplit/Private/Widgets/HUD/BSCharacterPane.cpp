// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSCharacterPane.h"

#include "Actors/Player/BSPlayerCharacter.h"
#include "Widgets/BSLocalWidgetSubsystem.h"

void UBSCharacterPane::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	if (ABSPlayerCharacter* BSPlayerCharacter = GetOwningPlayerPawn<ABSPlayerCharacter>())
	{
		BSPlayerCharacter->SetMenuCamera();
	}
}

void UBSCharacterPane::NativeOnDeactivated()
{
	if (ABSPlayerCharacter* BSPlayerCharacter = GetOwningPlayerPawn<ABSPlayerCharacter>())
	{
		BSPlayerCharacter->ResetCamera();
	}
	
	Super::NativeOnDeactivated();
}
