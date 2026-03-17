// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Abilities/BSCharacterInitData.h"

UBSCharacterInitData::UBSCharacterInitData()
{
	AttributeMap.Add(UDefaultAttributeSet::GetCurrentHealthAttribute(), 100.f);
	AttributeMap.Add(UDefaultAttributeSet::GetMaxHealthAttribute(), 100.f);
}

TSoftObjectPtr<UTexture2D> UBSCharacterInitData::GetIcon_Implementation() const
{
	return CharacterIcon;
}

FLinearColor UBSCharacterInitData::GetTint_Implementation() const
{
	return IconTint;
}
