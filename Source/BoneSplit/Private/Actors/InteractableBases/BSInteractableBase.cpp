// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/InteractableBases/BSInteractableBase.h"

#include "Components/WidgetComponent.h"

ABSInteractableBase::ABSInteractableBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>("WidgetComponent");
	WidgetComponent->SetupAttachment(GetRootComponent());
}

bool ABSInteractableBase::TryInteract(UBSInteractionComponent* InteractInstigator)
{
	return BP_TryInteract(InteractInstigator);
}

bool ABSInteractableBase::BP_TryInteract_Implementation(UBSInteractionComponent* InteractInstigator)
{
	return false;
}

void ABSInteractableBase::FocusInteractable(UBSInteractionComponent* InteractInstigator)
{
	BP_OnFocusedForInteraction(InteractInstigator);
}

void ABSInteractableBase::UnFocusInteractable(UBSInteractionComponent* InteractInstigator)
{
	BP_OnUnFocusForInteraction(InteractInstigator);
}

void ABSInteractableBase::BP_OnFocusedForInteraction_Implementation(UBSInteractionComponent* InteractInstigator)
{
}

void ABSInteractableBase::BP_OnUnFocusForInteraction_Implementation(UBSInteractionComponent* InteractInstigator)
{
}

bool ABSInteractableBase::IsInteractable(UBSInteractionComponent* InteractInstigator)
{
	return IsLocked() && BP_IsInteractable(InteractInstigator);
}

bool ABSInteractableBase::BP_IsInteractable_Implementation(UBSInteractionComponent* InteractInstigator)
{
	return false;
}

