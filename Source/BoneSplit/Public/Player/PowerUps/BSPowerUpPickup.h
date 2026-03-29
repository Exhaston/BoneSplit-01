// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPowerUpComponent.h"
#include "GameFramework/Actor.h"
#include "Player/Interactions/BSPlayerInteractionInterface.h"
#include "BSPowerUpPickup.generated.h"


class UBSWidget_PowerUpSelector;

UCLASS(Blueprintable, BlueprintType, Abstract, DisplayName="Power Up Pickup Actor", NotPlaceable)
class BONESPLIT_API ABSPowerUpPickup : public AActor, public IBSPlayerInteractionInterface
{
	GENERATED_BODY()

public:
	
	explicit ABSPowerUpPickup(const FObjectInitializer& ObjectInitializer);
	
	virtual void OnInteract_Implementation(AActor* InstigatorInteractor) override;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UBSWidget_PowerUpSelector> PowerUpSelectorClass;
	
	UPROPERTY()
	FBSAvailableChoice ChoiceData;
};
