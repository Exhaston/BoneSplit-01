// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Equipment/BSEquipmentAsset.h"

TSoftObjectPtr<UTexture2D> UBSEquipmentAsset::GetIcon_Implementation() const
{
	return EquipmentIcon;
}

FLinearColor UBSEquipmentAsset::GetTint_Implementation() const
{
	return IconTint;
}
