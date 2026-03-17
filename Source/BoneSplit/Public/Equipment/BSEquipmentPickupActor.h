// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSEquipmentComponent.h"
#include "BSEquipmentInstance.h"
#include "Actors/InteractableBases/BSInteractableBase.h"
#include "GameFramework/Actor.h"
#include "BSEquipmentPickupActor.generated.h"

class UBSEquipmentAsset;


UCLASS(Blueprintable, BlueprintType, Abstract)
class BONESPLIT_API ABSEquipmentPickupActor : public ABSInteractableBase
{
	GENERATED_BODY()

public:
	
	ABSEquipmentPickupActor(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;
	
	virtual bool TryInteract(UBSInteractionComponent* InteractInstigator) override;
	
#if WITH_EDITOR
	
	virtual void PostEditImport() override;
	
	virtual void PostActorCreated() override;

#endif
	
	UPROPERTY()
	FBSEquipPickupInfo ContainedEquipment;
	
protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UBSEquipmentAsset> EquipmentToGenerate;
	
};
