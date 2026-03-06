// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Targeting/BSAggroComponent.h"

#include "AbilitySystemInterface.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAbilityLibrary.h"
#include "Engine/OverlapResult.h"


UBSAggroComponent::UBSAggroComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1;
}

void UBSAggroComponent::CheckAggroSphere(const bool bCheckVisibility)
{
	const AController* OwnerController = GetOwnerController(); //Only valid of the server, same as authority check.
	if (!OwnerController) return;
	
	if (!OnTargetFoundDelegate.IsBound() || AggroRadius <= 0) return;
	
	const UWorld* World = OwnerController->GetWorld();
	if (!World) return;
	
	const IAbilitySystemInterface* OwnerAscInterface = Cast<IAbilitySystemInterface>(OwnerController->GetPawn());
	if (!OwnerAscInterface) return;
	
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(AggroRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerController->GetPawn());
	QueryParams.bTraceComplex = false;
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
		
	TArray<FOverlapResult> OverlapResults;
		
	World->OverlapMultiByObjectType(
		OverlapResults, 
		OwnerController->GetPawn()->GetActorLocation(), 
		FQuat::Identity,
		ObjectParams, 
		SphereShape, 
		QueryParams);

	for (auto& OverlapResult : OverlapResults)
	{
		AActor* FoundActor = OverlapResult.GetActor();
		if (!FoundActor) continue;
		
		if (FoundActors.ContainsByPredicate([this, FoundActor](const TWeakObjectPtr<AActor> TestActor)
		{ return TestActor == FoundActor; })) continue;
		
		if (bCheckVisibility && !OwnerController->LineOfSightTo(FoundActor)) continue;
		
		const IAbilitySystemInterface* TargetAscInterface = Cast<IAbilitySystemInterface>(FoundActor);
		if (!TargetAscInterface) continue;
		if (!TargetAscInterface->GetAbilitySystemComponent()) continue;

		const bool MatchingFaction = 
			UBSAbilityLibrary::HasMatchingFaction(
			OwnerAscInterface->GetAbilitySystemComponent(), 
			TargetAscInterface->GetAbilitySystemComponent());
			
		if (!MatchingFaction)
		{
			FoundActors.Add(FoundActor);
			OnTargetFoundDelegate.Broadcast(FoundActor);
		}
	}
	
#if WITH_EDITOR
	
	if (BS_AGGRO_DEBUG)
	{
		DrawDebugSphere(
			World, 
			OwnerController->GetPawn()->GetActorLocation(), 
			AggroRadius, 
			16, 
			FColor::Orange, 
			false, 
			PrimaryComponentTick.TickInterval);
	}
	
#endif
}

void UBSAggroComponent::TickComponent(
	const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (IsBeingDestroyed() || !GetWorld() || GetWorld()->bIsTearingDown) return;
	CheckAggroSphere(bVisibilityCheck);
}

AController* UBSAggroComponent::GetOwnerController()
{
	const APawn* OwnerPawn = GetOwner<APawn>();
	return OwnerPawn && OwnerPawn->GetController() ? OwnerPawn->GetController() : nullptr;
}

