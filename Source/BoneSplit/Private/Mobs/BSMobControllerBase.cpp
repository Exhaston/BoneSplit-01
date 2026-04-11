// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Mobs/BSMobControllerBase.h"

#include "ShapeOverlapBPLibrary.h"
#include "Factions/FactionInterface.h"

bool ABSMobControllerBase::LineOfSightTo(const AActor* Other, FVector ViewPoint, bool bAlternateChecks) const
{
	if (!GetPawn()) return false;
	return UShapeOverlapBPLibrary::LineOfSightToActor(GetPawn(), Other, 15, true);
}

bool ABSMobControllerBase::IsActorAlly(AActor* InActor)
{
	IFactionInterface* PawnFactionInterface = Cast<IFactionInterface>(GetPawn());
	IFactionInterface* TargetFactionInterface = Cast<IFactionInterface>(InActor);
	
	if (!TargetFactionInterface || !PawnFactionInterface) 
		return false;
	
	if (TargetFactionInterface->GetFactionTags().IsEmpty() || PawnFactionInterface->GetFactionTags().IsEmpty()) 
		return false;
	
	return PawnFactionInterface->HasAnyMatchingFactionTag(TargetFactionInterface->GetFactionTags());
}

bool ABSMobControllerBase::IsActorEnemy(AActor* InActor)
{
	IFactionInterface* PawnFactionInterface = Cast<IFactionInterface>(GetPawn());
	IFactionInterface* TargetFactionInterface = Cast<IFactionInterface>(InActor);
	
	if (!TargetFactionInterface || !PawnFactionInterface) 
		return false;
	
	if (TargetFactionInterface->GetFactionTags().IsEmpty() || PawnFactionInterface->GetFactionTags().IsEmpty()) 
		return false;
	
	return !PawnFactionInterface->HasAnyMatchingFactionTag(TargetFactionInterface->GetFactionTags());
}
