// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/InteractableBases/BSEquipmentDropBase.h"

#include "Actors/Player/BSPlayerState.h"
#include "Components/InteractionSystem/BSInteractionComponent.h"
#include "GameFramework/Character.h"


ABSEquipmentDropBase::ABSEquipmentDropBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABSEquipmentDropBase::InitializeLoot(const FBSLootSpawnInfo InLootSpawnInfo)
{
	LootSpawnInfo = InLootSpawnInfo;
}

bool ABSEquipmentDropBase::TryInteract(UBSInteractionComponent* InteractInstigator)
{
	if (const ACharacter* Character = Cast<ACharacter>(InteractInstigator->GetOwner()); 
		Character && Character->HasLocalNetOwner())
	{
		Character->GetPlayerState<ABSPlayerState>()->Server_EquipItem(LootSpawnInfo);
		Destroy();
		return true;
	}

	return false;
}

