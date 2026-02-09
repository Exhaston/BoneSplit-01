// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BSInteractableInterface.generated.h"

class UBSInteractionComponent;
class UAbilitySystemComponent;

UINTERFACE()
class UBSInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IBSInteractableInterface
{
	GENERATED_BODY()

public:
	
	virtual void FocusInteractable(UBSInteractionComponent* InteractInstigator) = 0;
	virtual void UnFocusInteractable(UBSInteractionComponent* InteractInstigator) = 0;
	virtual bool IsInteractable(UBSInteractionComponent* InteractInstigator) = 0;
	virtual bool TryInteract(UBSInteractionComponent* InteractInstigator) = 0;
};
