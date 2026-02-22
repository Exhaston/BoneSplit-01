// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/InteractableBases/BSEquipmentDropBase.h"

#include "Actors/Player/BSPlayerState.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/WidgetComponent.h"
#include "Components/InteractionSystem/BSInteractionComponent.h"
#include "Widgets/Inventory/BSEquipmentDropWidget.h"


ABSEquipmentDropBase::ABSEquipmentDropBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

#if WITH_EDITOR

void ABSEquipmentDropBase::PostEditImport()
{
	Super::PostEditImport();
	LootSpawnInfo.LootGuid = FGuid::NewGuid();
}

void ABSEquipmentDropBase::PostActorCreated()
{
	Super::PostActorCreated();
	LootSpawnInfo.LootGuid = FGuid::NewGuid();
}

#endif

void ABSEquipmentDropBase::BeginPlay()
{
	Super::BeginPlay();
	if (LootSpawnInfo.EquipmentEffect)
	{
		InitializeLoot(LootSpawnInfo);
	}
}

void ABSEquipmentDropBase::InitializeLoot(const FBSEquipmentDropInfo InLootSpawnInfo)
{
	LootSpawnInfo = InLootSpawnInfo;
	LootSpawnInfo.EquipmentTransform = GetActorTransform();
	if (!LootSpawnInfo.EquipmentEffect)
	{
		const FString LogFail = "No Valid Equipment on " + GetName() + ", destroying drop actor";
		UE_LOG(BoneSplit, Error, TEXT("%s"), *LogFail);
	}
	else if (WidgetComponent->GetUserWidgetObject() && 
		WidgetComponent->GetUserWidgetObject()->IsA(UBSEquipmentDropWidget::StaticClass()))
	{
		const UBSEquipmentEffect* EffectCDO = GetDefault<UBSEquipmentEffect>(LootSpawnInfo.EquipmentEffect);
		Cast<UBSEquipmentDropWidget>(WidgetComponent->GetUserWidgetObject())->InitializeEquipmentDropWidget(EffectCDO);
	}
}

bool ABSEquipmentDropBase::TryInteract(UBSInteractionComponent* InteractInstigator)
{
	if (!InteractInstigator) return false;
	if (IBSInventoryInterface* InventoryInterface = Cast<IBSInventoryInterface>(InteractInstigator->GetOwner()); InteractInstigator->GetOwner()->HasLocalNetOwner())
	{
		InventoryInterface->GetInventoryComponent()->Server_EquipItem(LootSpawnInfo, true);
		Destroy();
		return true;
	}
	
	return false;
}

