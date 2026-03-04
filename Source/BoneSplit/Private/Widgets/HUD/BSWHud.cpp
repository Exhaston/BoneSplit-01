// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSWHud.h"

#include "Actors/Player/BSPlayerController.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Widgets/BSLocalWidgetSubsystem.h"
#include "Widgets/HUD/BSAttributeBar.h"
#include "Widgets/HUD/BSActionButton.h"

void UBSWHud::InitializePlayerHUD(UBSAbilitySystemComponent* InAbilitySystemComponent)
{
	AbilitySystemComponent = InAbilitySystemComponent;
	if (AbilitySystemComponent.IsValid())
	{
		if (HealthBar) HealthBar->SetAttributeOwner(InAbilitySystemComponent);
	
		if (ManaBar) ManaBar->SetAttributeOwner(InAbilitySystemComponent);
	
		if (SoulBar) SoulBar->SetAttributeOwner(InAbilitySystemComponent);
		
		if (ActionButton1) ActionButton1->InitializeActionButton(InAbilitySystemComponent);
		
		if (ActionButton2) ActionButton2->InitializeActionButton(InAbilitySystemComponent);
		
		if (ActionButton3) ActionButton3->InitializeActionButton(InAbilitySystemComponent);
		
		if (ActionButton4) ActionButton4->InitializeActionButton(InAbilitySystemComponent);
		
		if (ActionButton5) ActionButton5->InitializeActionButton(InAbilitySystemComponent);
	}
}

void UBSWHud::NativePreConstruct()
{
	Super::NativePreConstruct();
	
}

void UBSWHud::NativeConstruct()
{
	Super::NativeConstruct();
	

}

void UBSWHud::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	if (ABSPlayerController* OwningPS = GetOwningPlayer<ABSPlayerController>())
	{

	}
}
