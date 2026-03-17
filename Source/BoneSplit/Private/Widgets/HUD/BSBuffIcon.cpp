// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSBuffIcon.h"

#include "Abilities/BSBuffEffect.h"

void UBSBuffIcon::InitializeBuffIcon(const UBSBuffEffect* BuffEffect)
{
	BuffIconImage->SetBrushFromLazyTexture(BuffEffect->GetIcon_Implementation());
	BuffIconImage->SetBrushTintColor(BuffEffect->GetTint_Implementation());
}
