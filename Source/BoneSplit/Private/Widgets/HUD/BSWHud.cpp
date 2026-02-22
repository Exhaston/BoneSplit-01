// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSWHud.h"

#include "Actors/Player/BSPlayerState.h"
#include "Widgets/BSLocalWidgetSubsystem.h"
#include "Widgets/HUD/BSAttributeBar.h"
#include "Widgets/HUD/BSActionButton.h"

void UBSWHud::NativePreConstruct()
{
	Super::NativePreConstruct();
	
}

void UBSWHud::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (const ABSPlayerState* OwningPS = GetOwningPlayerState<ABSPlayerState>())
	{
		AbilitySystemComponent = OwningPS->GetAbilitySystemComponent();
		if (AbilitySystemComponent.IsValid())
		{
			if (HealthBar) HealthBar->SetAttributeOwner(AbilitySystemComponent.Get());
	
			if (ManaBar) ManaBar->SetAttributeOwner(AbilitySystemComponent.Get());
	
			if (SoulBar) SoulBar->SetAttributeOwner(AbilitySystemComponent.Get());
		}
	}
}
