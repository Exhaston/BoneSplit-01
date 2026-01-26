// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Targeting/BSAggroComponent.h"

#include "AbilitySystemInterface.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAbilityLibrary.h"
#include "Engine/OverlapResult.h"


UBSAggroComponent::UBSAggroComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 2;
}

void UBSAggroComponent::CheckAggroSphere(const bool bCheckVisibility)
{
	if (!OnTargetFoundDelegate.IsBound() || AggroRadius <= 0) return;
	
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;
	
	const IAbilitySystemInterface* OwnerAscInterface = Cast<IAbilitySystemInterface>(OwnerActor);
	if (!OwnerAscInterface) return;
	
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(AggroRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerActor);
	QueryParams.bTraceComplex = false;
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
		
	TArray<FOverlapResult> OverlapResults;
		
	OwnerActor->GetWorld()->OverlapMultiByObjectType(
		OverlapResults, 
		OwnerActor->GetActorLocation(), 
		FQuat::Identity,
		ObjectParams, 
		SphereShape, 
		QueryParams);

	for (auto& OverlapResult : OverlapResults)
	{
		AActor* FoundActor = OverlapResult.GetActor();
		if (!FoundActor) continue;
			
		if (bCheckVisibility)
		{
			if (!UBSAbilityLibrary::CheckTargetVisibility(
				OwnerActor, OwnerActor->GetActorLocation(), FoundActor))
			{
				continue;
			}
		}
			
		const IAbilitySystemInterface* TargetAscInterface = Cast<IAbilitySystemInterface>(FoundActor);
		if (!TargetAscInterface) continue;

		const bool MatchingFaction = 
			UBSAbilityLibrary::HasMatchingFaction(
			OwnerAscInterface->GetAbilitySystemComponent(), 
			TargetAscInterface->GetAbilitySystemComponent());
			
		if (!MatchingFaction)
		{
			OnTargetFoundDelegate.Broadcast(FoundActor);
		}
	}
	
#if WITH_EDITOR
	
	if (BS_HIT_DEBUG)
	{
		DrawDebugSphere(
			OwnerActor->GetWorld(), 
			OwnerActor->GetActorLocation(), 
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
	
	if (IsBeingDestroyed()) return;
	CheckAggroSphere();
}

