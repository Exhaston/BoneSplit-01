// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BSEquipmentInterface.generated.h"

struct FBSEquipPickupInfo;
class UBSEquipmentComponent;
// This class does not need to be modified.
UINTERFACE()
class UBSEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BONESPLIT_API IBSEquipmentInterface
{
	GENERATED_BODY()

public:
	
	virtual UBSEquipmentComponent* GetEquipmentComponent() const = 0;
	virtual void ApplyEquipment(const FBSEquipPickupInfo& Pickup) = 0;
};
