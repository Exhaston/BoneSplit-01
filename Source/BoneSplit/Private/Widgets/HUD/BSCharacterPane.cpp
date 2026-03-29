// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSCharacterPane.h"

#include "Player/BSPlayerCharacterBase.h"
#include "Widgets/BSLocalWidgetSubsystem.h"

void UBSCharacterPane::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	if (ABSPlayerCharacterBase* BSPlayerCharacter = GetOwningPlayerPawn<ABSPlayerCharacterBase>())
	{
		BSPlayerCharacter->SetMenuCamera();
	}
}

void UBSCharacterPane::NativeOnDeactivated()
{
	if (ABSPlayerCharacterBase* BSPlayerCharacter = GetOwningPlayerPawn<ABSPlayerCharacterBase>())
	{
		BSPlayerCharacter->ResetCamera();
	}
	
	Super::NativeOnDeactivated();
}
