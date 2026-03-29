// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/Interactions/BSPlayerInteractionComponent.h"

#include "Engine/OverlapResult.h"
#include "Player/Interactions/BSPlayerInteractionInterface.h"


UBSPlayerInteractionComponent::UBSPlayerInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.5f;
}

UBSPlayerInteractionComponent* UBSPlayerInteractionComponent::GetInteractionComponent(const AActor* InTarget)
{
    return InTarget->GetComponentByClass<UBSPlayerInteractionComponent>();
}

void UBSPlayerInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UBSPlayerInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!GetOwner() || !GetOwner()->HasLocalNetOwner()) return;
    UpdateFocus();
}

void UBSPlayerInteractionComponent::UpdateFocus()
{
    AActor* NewFocus = FindClosestInteractable();

    if (NewFocus == FocusedActor)
        return;

    // Notify the old actor it's lost focus
    if (FocusedActor && FocusedActor->Implements<UBSPlayerInteractionInterface>())
    {
        IBSPlayerInteractionInterface::Execute_OnFocusLost(FocusedActor, GetOwner());
    }

    FocusedActor = NewFocus;

    // Notify the new actor it's gained focus
    if (FocusedActor && FocusedActor->Implements<UBSPlayerInteractionInterface>())
    {
        IBSPlayerInteractionInterface::Execute_OnFocusGained(FocusedActor, GetOwner());
    }
}

AActor* UBSPlayerInteractionComponent::FindClosestInteractable() const
{
    const FVector Origin = GetOwner()->GetActorLocation();

    TArray<FOverlapResult> Overlaps;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractionRadius);
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    GetWorld()->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity,
                                      InteractionChannel, Sphere, Params);

    AActor* Closest = nullptr;
    float   ClosestDistSq = FLT_MAX;

    for (const FOverlapResult& Hit : Overlaps)
    {
        AActor* Actor = Hit.GetActor();
        if (!Actor || !Actor->Implements<UBSPlayerInteractionInterface>())
            continue;

        const float DistSq = 
            FVector::DistSquared(Origin, Actor->GetActorLocation());
        
        if (DistSq < ClosestDistSq)
        {
            ClosestDistSq = DistSq;
            Closest       = Actor;
        }
    }

    return Closest;
}

void UBSPlayerInteractionComponent::TryInteract() const
{
    if (!FocusedActor || !FocusedActor->Implements<UBSPlayerInteractionInterface>())
        return;

    IBSPlayerInteractionInterface::Execute_OnInteract(FocusedActor, GetOwner());
}


