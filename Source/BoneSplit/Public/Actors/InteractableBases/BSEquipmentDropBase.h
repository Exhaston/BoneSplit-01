// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSInteractableBase.h"
#include "Components/Inventory/BSInventoryComponent.h"
#include "BSEquipmentDropBase.generated.h"

class ABSPlayerState;
																			   
//Loot drop base that should be subclassed and spawned through the equipment component or pre-placed in the world.
//Flow would be -> SpawnDeferred -> InitializeLoot -> FinishSpawning, 
//Equipment component handles this automatically with the preset subclass of this in project settings. 
UCLASS(Abstract, BlueprintType, Blueprintable, DisplayName="EquipmentLootDrop")
class BONESPLIT_API ABSEquipmentDropBase : public ABSInteractableBase
{
	GENERATED_BODY()

public:
	
	explicit ABSEquipmentDropBase(const FObjectInitializer& ObjectInitializer);
	
#if WITH_EDITOR

	virtual void PostEditImport() override;
	
	virtual void PostActorCreated() override;
	
#endif
	
	virtual void BeginPlay() override;
	
	virtual void InitializeLoot(FBSEquipmentDropInfo InLootSpawnInfo);
	
	virtual bool TryInteract(UBSInteractionComponent* InteractInstigator) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBSEquipmentDropInfo LootSpawnInfo;
};
