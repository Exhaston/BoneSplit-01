// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/BSWHud.h"

#include "GameFramework/PlayerState.h"
#include "Widgets/BSAttributeBar.h"

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
}

UAbilitySystemComponent* UBSWHud::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.IsValid() ? AbilitySystemComponent.Get() : nullptr;
}
