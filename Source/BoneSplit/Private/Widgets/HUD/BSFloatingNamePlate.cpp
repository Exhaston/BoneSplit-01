// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSFloatingNamePlate.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Widgets/HUD/BSAttributeBar.h"

void UBSFloatingNamePlate::InitializeAbilitySystemComponent(UAbilitySystemComponent* InAsc)
{
	HealthBarWidget->SetAttributeOwner(InAsc);
}
