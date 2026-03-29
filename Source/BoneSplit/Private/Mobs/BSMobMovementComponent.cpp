// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Mobs/BSMobMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/BSExtendedAttributeSet.h"


UBSMobMovementComponent::UBSMobMovementComponent()
{
	bWantsInitializeComponent = true;
}

void UBSMobMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (const IAbilitySystemInterface* OwnerAscI = Cast<IAbilitySystemInterface>(GetOwner()))
		OwnerAbilitySystem = OwnerAscI->GetAbilitySystemComponent();
}

float UBSMobMovementComponent::GetSpeedModifier() const
{
	return OwnerAbilitySystem.IsValid() ? 
	OwnerAbilitySystem->GetNumericAttribute(UBSExtendedAttributeSet::GetSpeedAttribute()) : Super::GetSpeedModifier();
}
