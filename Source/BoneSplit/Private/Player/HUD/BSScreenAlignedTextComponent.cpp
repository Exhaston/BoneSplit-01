// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/HUD/BSScreenAlignedTextComponent.h"


UBSScreenAlignedTextComponent::UBSScreenAlignedTextComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetUsingAbsoluteRotation(true);
	
	HorizontalAlignment = EHTA_Center;
	VerticalAlignment = EVRTA_TextCenter;
}

#if WITH_EDITOR
bool UBSScreenAlignedTextComponent::CanEditChange(const FProperty* InProperty) const
{
	if (InProperty->GetFName() == FName("RelativeRotation"))
	{
		return false;
	}
	
	if (InProperty->GetFName() == FName("bAbsoluteRotation"))
	{
		return false;
	}
	
	return Super::CanEditChange(InProperty);
}
#endif

