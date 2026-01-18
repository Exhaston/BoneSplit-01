// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSLockableActor.h"
#include "GameFramework/Actor.h"
#include "Actors/InteractableBases/BSInteractableInterface.h"
#include "BSInteractableBase.generated.h"                                                          

UCLASS(DisplayName="Interactable Base", Category="BoneSplit")
class BONESPLIT_API ABSInteractableBase : public ABSLockableActor, public IBSInteractableInterface
{
	GENERATED_BODY()

public:
	explicit ABSInteractableBase(const FObjectInitializer& ObjectInitializer);
};
