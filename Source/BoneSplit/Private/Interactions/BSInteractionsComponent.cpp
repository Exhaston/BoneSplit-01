// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Interactions/BSInteractionsComponent.h"

#include "Interactions/BSInteractionInterface.h"

UBSInteractionsComponent::UBSInteractionsComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBSInteractionsComponent::RegisterInteractable(const FGuid NewInteractableGuid)
{
	RegisteredInteractions.Add(NewInteractableGuid);
}

void UBSInteractionsComponent::RequestInteraction(const FGuid& InteractableGuid)
{
}

void UBSInteractionsComponent::Server_RequestInteraction_Implementation(const FGuid& Guid)
{
	if (IsRegistered(Guid))
	{
		Client_AcceptInteraction(Guid);
		UnRegister(Guid);
	}
}

void UBSInteractionsComponent::UnRegister(const FGuid& Guid)
{
	RegisteredInteractions.Remove(Guid);
}

void UBSInteractionsComponent::Client_AcceptInteraction_Implementation(const FGuid& Guid)
{
	OnInteractionAccepted.Broadcast(Guid);
}

void UBSInteractionsComponent::Client_RejectInteraction_Implementation(const FGuid& Guid)
{
	OnInteractionRejected.Broadcast(Guid);
}

bool UBSInteractionsComponent::IsRegistered(const FGuid& Guid) const
{
	return RegisteredInteractions.Contains(Guid);
}

void UBSInteractionsComponent::BeginPlay()
{
	Super::BeginPlay();
}

