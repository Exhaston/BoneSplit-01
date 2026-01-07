// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/BSDynamicDecalComponent.h"


UBSDynamicDecalComponent::UBSDynamicDecalComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBSDynamicDecalComponent::RegisterDynamicDecal(UMaterialInterface* InMaterial)
{
	SetMaterial(0, InMaterial);
	DynamicMaterial = CreateDynamicMaterialInstance();
	RegisterComponent();
}

UMaterialInstanceDynamic* UBSDynamicDecalComponent::GetDynamicMaterial() const
{
	return DynamicMaterial;
}

