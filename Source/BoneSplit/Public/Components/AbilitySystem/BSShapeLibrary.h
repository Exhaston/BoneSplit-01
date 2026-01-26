// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BSShapeLibrary.generated.h"

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSShapeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	                                                                    
public:
	
	//Returns actors that overlapped the wedge/slice shape. Will include self if collision channel is a match.
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Shape", DisplayName="Wedge Overlap Actors", meta=(DefaultToSelf="WorldContext"))
	static TArray<AActor*> BP_OverlapShapeWedge(
		UObject* WorldContext, 
		FTransform ParentTransform, 
		float Distance, 
		float Height, 
		float Angle, 
		TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels,
		bool bCheckVisibility = true);
	
	static TArray<AActor*> OverlapShapeWedge(
		UWorld* World, 
		FTransform ParentTransform, 
		float Distance, 
		float Height, 
		float Angle, 
		TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels,
		bool bCheckVisibility = true);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Shape", DisplayName="Draw Debug Wedge", meta=(DefaultToSelf="WorldContext"))
	static void BP_DebugDrawWedge(
		UObject* WorldContext, 
		FTransform ParentTransform, 
		float Distance, 
		float Height, 
		float Angle, 
		float Duration = 2, 
		FColor Color = FColor::Red,
		bool bDrawBounds = false);
	
	static void DebugDrawWedge(
		UWorld* World, 
		FTransform ParentTransform, 
		float Distance, 
		float Height, 
		float Angle, 
		float Duration = 2, 
		FColor Color = FColor::Red,
		bool bDrawBounds = false);
	
	//Cheapest overlap option
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Shape", DisplayName="Sphere Overlap Actors", meta=(DefaultToSelf="WorldContext"))
	static TArray<AActor*> BP_SphereOverlapActors(
		UObject* WorldContext, 
		FTransform ParentTransform, 
		float Radius,
		TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels,
		bool bCheckVisibility = true);
	
	static TArray<AActor*> SphereOverlapActors(
		const UWorld* World,
		const FTransform& ParentTransform, 
		float Radius, 
		TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels,
		bool bCheckVisibility = true);
	
	//Shoots a ray between the origin and target points. 
	//If it hits any geometry tagged as ECC_Visibility it will count as not visible
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Shape", 
		DisplayName="Check Visibility", meta=(DefaultToSelf="WorldContext"))
	static bool BP_IsVisible(
		UObject* WorldContext, 
		FVector OriginPoint, 
		FVector TargetPoint);
	
	static bool IsVisible(
		const UWorld* World,
		const FVector& OriginPoint,
		const FVector& TargetPoint);
};
