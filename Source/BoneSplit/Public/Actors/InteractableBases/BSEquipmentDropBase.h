// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSInteractableBase.h"
#include "GameState/BSGameState.h"
#include "BSEquipmentDropBase.generated.h"

class ABSPlayerState;

UCLASS(NotPlaceable, BlueprintType, Blueprintable, DisplayName="EquipmentLootDrop")
class BONESPLIT_API ABSEquipmentDropBase : public ABSInteractableBase
{
	GENERATED_BODY()

public:
	
	ABSEquipmentDropBase(const FObjectInitializer& ObjectInitializer);
	
	virtual void InitializeLoot(FBSLootSpawnInfo InLootSpawnInfo);
	
	virtual bool TryInteract(UBSInteractionComponent* InteractInstigator) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBSLootSpawnInfo LootSpawnInfo;
};
