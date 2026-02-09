// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSWHud.h"

#include "GameFramework/PlayerState.h"
#include "Widgets/HUD/BSAttributeBar.h"
#include "Widgets/HUD/BSActionButton.h"

void UBSWHud::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (ActionButton1) ActionButton1->InputID = 1;
	if (ActionButton2) ActionButton2->InputID = 2;
	if (ActionButton3) ActionButton3->InputID = 3;
	if (ActionButton4) ActionButton4->InputID = 4;
	if (ActionButton5) ActionButton5->InputID = 5;
}

void UBSWHud::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (GetOwningPlayerState())
	{
		if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(GetOwningPlayerState<APlayerState>()))
		{
			InitializeHud(AscInterface->GetAbilitySystemComponent());
		}
	}
}

void UBSWHud::InitializeHud(UAbilitySystemComponent* InAbilitySystemComponent)
{
	check(InAbilitySystemComponent);
	AbilitySystemComponent = InAbilitySystemComponent;
	
	if (HealthBar)
	{
		HealthBar->InitializeAttributeBar(GetAbilitySystemComponent());
	}
	
	if (ManaBar)
	{
		 ManaBar->InitializeAttributeBar(GetAbilitySystemComponent());
	}
	
	if (SoulBar)
	{
		SoulBar->InitializeAttributeBar(GetAbilitySystemComponent());
	}
	
	if (ActionButton1)
	{
		ActionButton1->InitializeActionButton(GetAbilitySystemComponent());
	}    
	
	if (ActionButton2)
	{
		ActionButton2->InitializeActionButton(GetAbilitySystemComponent());
	}
	
	if (ActionButton3)
	{
		ActionButton3->InitializeActionButton(GetAbilitySystemComponent());
	}
	
	if (ActionButton4)
	{
		ActionButton4->InitializeActionButton(GetAbilitySystemComponent());
	}
	
	if (ActionButton5)
	{
		ActionButton5->InitializeActionButton(GetAbilitySystemComponent());
	}
}

UAbilitySystemComponent* UBSWHud::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.IsValid() ? AbilitySystemComponent.Get() : nullptr;
}
