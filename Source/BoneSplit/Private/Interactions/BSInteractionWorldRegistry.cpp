// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Interactions/BSInteractionWorldRegistry.h"

FGuid UBSInteractionWorldRegistry::RegisterInteractable(const FGuid NewInteractableGuid)
{                          
	return RegisteredInteractionGuids.Add_GetRef(NewInteractableGuid);
}

void UBSInteractionWorldRegistry::UnRegister(const FGuid& Guid)
{
	RegisteredInteractionGuids.Remove(Guid);
}

bool UBSInteractionWorldRegistry::IsRegistered(const FGuid& Guid) const
{
	return RegisteredInteractionGuids.Contains(Guid);
}
