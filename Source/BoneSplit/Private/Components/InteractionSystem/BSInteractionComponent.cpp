// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/InteractionSystem/BSInteractionComponent.h"

#include "BoneSplit/BoneSplit.h"
#include "Components/InteractionSystem/BSInteractableInterface.h"
#include "Engine/OverlapResult.h"


UBSInteractionComponent::UBSInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	PrimaryComponentTick.TickInterval = 0.25;
}

void UBSInteractionComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsValid(this)) return;
	if (!GetOwner()) return;
	if (GetOwner()->IsPendingKillPending()) return;
	if (!GetOwner()->HasLocalNetOwner()) return;
	
	const UWorld* World = GetOwner()->GetWorld();
	

	
	//Add all interactions in radius
	
	TArray<FOverlapResult> OverlapResults;
	World->OverlapMultiByObjectType(
		OverlapResults, 
		GetOwner()->GetActorLocation(), 
		FQuat::Identity, 
		ECC_WorldDynamic, FCollisionShape::MakeSphere(InteractionRadius));
	
	TArray<TWeakObjectPtr<AActor>> OverlapActors;
	
	float ClosestDistance = FLT_MAX;
	AActor* ClosestActor = nullptr;

	for (auto& OverlapResult : OverlapResults)
	{
		if (OverlapResult.GetActor() && OverlapResult.GetActor()->Implements<UBSInteractableInterface>())
		{

			const float Distance = FVector::DistSquared(
			GetOwner()->GetActorLocation(), OverlapResult.GetActor()->GetActorLocation());
			
			if (Distance < ClosestDistance)
			{
				ClosestActor = OverlapResult.GetActor();
				ClosestDistance = Distance;
			}
		}
	}
	
	if (CurrentFocus != ClosestActor)
	{
		if (CurrentFocus.IsValid()) //Broadcast to old interaction if there was any
		{
			Cast<IBSInteractableInterface>(CurrentFocus.Get())->UnFocusInteractable(this);
		}
		
		CurrentFocus = ClosestActor;
		
		OnFocusChanged.Broadcast(ClosestActor);
		
		if (CurrentFocus.IsValid())
		{
			Cast<IBSInteractableInterface>(CurrentFocus.Get())->FocusInteractable(this);
		}
	}
}

bool UBSInteractionComponent::TryInteract()
{
	if (GetCurrentInteractable())
	{
		return Cast<IBSInteractableInterface>(GetCurrentInteractable())->TryInteract(this);
	}
	
	UE_LOG(BoneSplit, Display, TEXT("Nothing to interact with"));
	
	return false;
}

AActor* UBSInteractionComponent::GetCurrentInteractable() const
{
	return CurrentFocus.IsValid() ? CurrentFocus.Get() : nullptr;
}

