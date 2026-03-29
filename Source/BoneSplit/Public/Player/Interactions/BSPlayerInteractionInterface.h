// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BSPlayerInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UBSPlayerInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BONESPLIT_API IBSPlayerInteractionInterface
{
	GENERATED_BODY()

public:
	
	// Called when this actor becomes the closest interactable
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void OnFocusGained(AActor* InstigatorInteractor);

	// Called when a closer interactable is found (or player moves away)
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void OnFocusLost(AActor* InstigatorInteractor);

	// Called when the player presses the interact input while focused on this actor
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void OnInteract(AActor* InstigatorInteractor);
};
