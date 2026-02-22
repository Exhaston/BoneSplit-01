// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/FloatingUnitPlates/BSFloatingUnitPlateSubsystem.h"

#include "Components/WidgetComponent.h"

bool UBSFloatingUnitPlateSubsystem::IsTickable() const
{
	return !IsTemplate();
}

TStatId UBSFloatingUnitPlateSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBSFloatingUnitPlateSubsystem, STATGROUP_Tickables);
}

void UBSFloatingUnitPlateSubsystem::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	for (auto It = RegisteredFloatingNamePlates.CreateIterator(); It; ++It)
	{
		if (!It->IsValid() || !It->Get()->GetOwner())
		{
			It.RemoveCurrent();
		}
	}
}

void UBSFloatingUnitPlateSubsystem::RegisterFloatingNamePlate(UWidgetComponent* FloatingNamePlate)
{
	if (!FloatingNamePlate) return;
	RegisteredFloatingNamePlates.Add(FloatingNamePlate);
}

void UBSFloatingUnitPlateSubsystem::UnRegisterFloatingNamePlate(UWidgetComponent* FloatingNamePlate)
{
	if (!FloatingNamePlate) return;
	RegisteredFloatingNamePlates.Remove(FloatingNamePlate);
}
