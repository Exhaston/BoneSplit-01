// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/UnitPlateManager/BSUnitPlateManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/AbilitySystem/BSAbilityLibrary.h"
#include "Engine/OverlapResult.h"
#include "Widgets/HUD/BSFloatingNamePlate.h"
#include "Widgets/BSLocalWidgetSubsystem.h"

UBSUnitPlateManagerComponent::UBSUnitPlateManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void UBSUnitPlateManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!GetOwner() || GetOwner()->IsActorBeingDestroyed()) return;

    APlayerController* PC = GetOwner<APlayerController>();
    if (!PC || !PC->IsLocalController()) return;
    
    TSet<UAbilitySystemComponent*> InRangeComps;

    FVector Origin;
    FRotator Rotation;
    
    PC->GetPlayerViewPoint(Origin, Rotation);
    
    TArray<FOverlapResult> Overlaps;
    GetOwner()->GetWorld()->OverlapMultiByObjectType(
        Overlaps,
        Origin,
        FQuat::Identity,
        FCollisionObjectQueryParams(ECC_Pawn),
        FCollisionShape::MakeSphere(2500)
    );

    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* Actor = Overlap.GetActor();
        if (!Actor || Actor == PC->GetPawn()) continue;

        if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(Actor))
        {
            if (UAbilitySystemComponent* ASC = AscInterface->GetAbilitySystemComponent())
            {
                InRangeComps.Add(ASC);
            }
        }
    }
    
    for (UAbilitySystemComponent* ASC : InRangeComps)
    {
        if (!UnitPlateInstances.Contains(ASC))
        {
            UBSFloatingNamePlate* NewInstance = CreateWidget<UBSFloatingNamePlate>(PC, NamePlateWidgetClass);
            UnitPlateInstances.Add(ASC, NewInstance);
            NewInstance->InitializeAbilitySystemComponent(ASC);
            NewInstance->AddToPlayerScreen();
        }
    }
    
    for (auto It = UnitPlateInstances.CreateIterator(); It; ++It)
    {
        const UAbilitySystemComponent* Asc = It->Key;
        UBSFloatingNamePlate* Instance = It->Value;

        const bool bVisible = Asc->GetAvatarActor() && UBSAbilityLibrary::CanSeeTargetLocation(GetOwner()->GetWorld(), Origin, Asc->GetAvatarActor()->GetActorLocation());
        const bool bInvalid = !IsValid(Asc) || !IsValid(Instance) || !bVisible;

        if (bInvalid || !InRangeComps.Contains(Asc))
        {
            if (IsValid(Instance))
            {
                Instance->RemoveFromParent();
            }
            It.RemoveCurrent(); // Safe with CreateIterator()
        }
        else
        {
            Instance->SetLocation();
        }
    }
}

