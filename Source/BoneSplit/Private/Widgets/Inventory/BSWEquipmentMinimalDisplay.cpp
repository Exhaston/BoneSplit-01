// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Inventory/BSWEquipmentMinimalDisplay.h"

#include "Actors/Player/BSPlayerState.h"

void UBSWEquipmentMinimalDisplay::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ABSPlayerState* PS = GetOwningPlayerState<ABSPlayerState>())
	{
		for (const auto ExistingEquipment : PS->GetInventoryComponent()->GetEquipment())
		{
			if (ExistingEquipment->SlotTag.MatchesTagExact(EquipmentSlotToTrack))
			{
				EquipmentIconImage->SetBrushFromLazyTexture(ExistingEquipment->Icon);
			}
		}
		
		PS->GetInventoryComponent()->GetEquipmentEquippedDelegate().AddWeakLambda(
			this, [this](const UBSEquipmentEffect* Equipment)
		{
				EquipmentIconImage->SetBrushFromLazyTexture(Equipment->Icon);
		});
	}
}
