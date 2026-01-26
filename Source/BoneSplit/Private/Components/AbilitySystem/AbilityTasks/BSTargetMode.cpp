// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityTasks/BSTargetMode.h"

#include "Components/AbilitySystem/BSAbilityLibrary.h"

//TODO many of these array iterations could be optimized with some elbow grease.

AActor* UBSTargetMode_RandomThreat::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityLibrary::GetThreatActors(InAvatarActor);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityLibrary::PickRandomTargetFromArray(Targets);
}

AActor* UBSTargetMode_HighestThreat::GetTarget(AActor* InAvatarActor)
{
	return UBSAbilityLibrary::GetHighestThreatActor(InAvatarActor);
}

AActor* UBSTargetMode_LowestThreat::GetTarget(AActor* InAvatarActor)
{
	return UBSAbilityLibrary::GetLowestThreatActor(InAvatarActor);
}

AActor* UBSTargetMode_RandomNearby::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityLibrary::GetActorsInRadiusDep(InAvatarActor, Radius);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityLibrary::PickRandomTargetFromArray(Targets);
}

AActor* UBSTargetMode_Closest::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityLibrary::GetActorsInRadiusDep(InAvatarActor, Radius);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityLibrary::PickClosestTargetFromArray(InAvatarActor, Targets);
}

AActor* UBSTargetMode_Farthest::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityLibrary::GetActorsInRadiusDep(InAvatarActor, Radius);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityLibrary::PickFarthestTargetFromArray(InAvatarActor, Targets);
}

AActor* UBSTargetMode_ClosestThreat::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityLibrary::GetThreatActors(InAvatarActor);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityLibrary::PickFarthestTargetFromArray(InAvatarActor, Targets);
}

AActor* UBSTargetMode_FarthestThreat::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityLibrary::GetThreatActors(InAvatarActor);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityLibrary::PickFarthestTargetFromArray(InAvatarActor, Targets);
}
