// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/Equipment/BSEquipmentPickupActor.h"

#include "Components/WidgetComponent.h"
#include "Player/Equipment/BSEquipmentInterface.h"
#include "Widgets/Inventory/BSEquipmentDropWidget.h"


ABSEquipmentPickupActor::ABSEquipmentPickupActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABSEquipmentPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	/*
	if (ContainedEquipment.EquipmentInstance.HasValidData() && WidgetComponent->GetUserWidgetObject() && 
	WidgetComponent->GetUserWidgetObject()->IsA(UBSEquipmentDropWidget::StaticClass()))
	{
		Cast<UBSEquipmentDropWidget>(WidgetComponent->GetUserWidgetObject())->InitializeEquipmentDropWidget(ContainedEquipment.EquipmentInstance);
	}
	*/
}

void ABSEquipmentPickupActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (!ContainedEquipment.EquipmentInstance.HasValidData() && EquipmentToGenerate)
	{
		ContainedEquipment.EquipmentInstance = FBSEquipmentInstance(EquipmentToGenerate);
	}
}

/*
bool ABSEquipmentPickupActor::TryInteract(UBSInteractionComponent* InteractInstigator)
{
	if (!InteractInstigator) return false;
	
	AActor* InstigatorOwnerActor = InteractInstigator->GetOwner();
	if (!InstigatorOwnerActor->HasLocalNetOwner()) return false;
	
	IBSEquipmentInterface* OwnerEquipmentInterface = Cast<IBSEquipmentInterface>(InstigatorOwnerActor);
	
	if (!OwnerEquipmentInterface) return false;
	
	OwnerEquipmentInterface->ApplyEquipment(ContainedEquipment);
	
	Destroy();
	
	return true;
}
*/

#if WITH_EDITOR

void ABSEquipmentPickupActor::PostEditImport()
{
	Super::PostEditImport();
	ContainedEquipment.EquipmentPickupInfo = FGuid::NewGuid();
}

void ABSEquipmentPickupActor::PostActorCreated()
{
	Super::PostActorCreated();
	ContainedEquipment.EquipmentPickupInfo = FGuid::NewGuid();
}

#endif

