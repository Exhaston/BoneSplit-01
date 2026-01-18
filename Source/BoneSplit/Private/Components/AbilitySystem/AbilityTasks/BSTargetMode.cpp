// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityTasks/BSTargetMode.h"

#include "Components/AbilitySystem/BSAbilityFunctionLibrary.h"

//TODO many of these array iterations could be optimized with some elbow grease.

AActor* UBSTargetMode_RandomThreat::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityFunctionLibrary::GetThreatActors(InAvatarActor);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityFunctionLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityFunctionLibrary::PickRandomTargetFromArray(Targets);
}

AActor* UBSTargetMode_HighestThreat::GetTarget(AActor* InAvatarActor)
{
	return UBSAbilityFunctionLibrary::GetHighestThreatActor(InAvatarActor);
}

AActor* UBSTargetMode_LowestThreat::GetTarget(AActor* InAvatarActor)
{
	return UBSAbilityFunctionLibrary::GetLowestThreatActor(InAvatarActor);
}

AActor* UBSTargetMode_RandomNearby::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityFunctionLibrary::GetActorsInRadius(InAvatarActor, Radius);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityFunctionLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityFunctionLibrary::PickRandomTargetFromArray(Targets);
}

AActor* UBSTargetMode_Closest::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityFunctionLibrary::GetActorsInRadius(InAvatarActor, Radius);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityFunctionLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityFunctionLibrary::PickClosestTargetFromArray(InAvatarActor, Targets);
}

AActor* UBSTargetMode_Farthest::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityFunctionLibrary::GetActorsInRadius(InAvatarActor, Radius);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityFunctionLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityFunctionLibrary::PickFarthestTargetFromArray(InAvatarActor, Targets);
}

AActor* UBSTargetMode_ClosestThreat::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityFunctionLibrary::GetThreatActors(InAvatarActor);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityFunctionLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityFunctionLibrary::PickFarthestTargetFromArray(InAvatarActor, Targets);
}

AActor* UBSTargetMode_FarthestThreat::GetTarget(AActor* InAvatarActor)
{
	TArray<AActor*> Targets = UBSAbilityFunctionLibrary::GetThreatActors(InAvatarActor);
	
	if (bFilterByReachable)
	{
		Targets = UBSAbilityFunctionLibrary::FilterArrayByReachable(InAvatarActor, Targets);
	}
	
	return UBSAbilityFunctionLibrary::PickFarthestTargetFromArray(InAvatarActor, Targets);
}
