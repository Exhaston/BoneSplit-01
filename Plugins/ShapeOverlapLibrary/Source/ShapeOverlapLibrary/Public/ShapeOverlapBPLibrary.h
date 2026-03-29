// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShapeOverlapBPLibrary.generated.h"


UCLASS(DisplayName="Shape Overlap Library")
class UShapeOverlapBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	                                                                                                                
public:
	
	//Performs a very simple visibility line trace. Works well if none of the dynamic actors uses visibility blocking.
	//UFUNCTION(BlueprintCallable, DisplayName="Line Of Sight To", meta=(Keywords = "LOS, Line, Sight, Visiblity, Visible"), Category = "ShapeOverlapLibrary")
	static bool LineOfSightTo(UObject* WorldContext, FVector Origin, FVector Target);
	
	/**
	 * Performs a wedge-shaped overlap check from the transform location in its forward direction.
	 * More expensive than a standard shape overlaps — use only when precise angular filtering is required.
	 *
	 * @param WorldContext		World context object (auto-bound to self in Blueprints).
	 * @param OverlappedActors	Output array of actors found within the wedge volume.
	 * @param ParentTransform	Origin and orientation of the wedge. Forward vector defines the facing direction.
	 * @param Distance		Radius of the wedge along the forward axis.
	 * @param Height		Total height of the wedge volume along the up axis.
	 * @param Angle			Full angular width of the wedge in degrees (e.g. 90 = 45 degrees either side of forward).
	 * @param bUseOverlapComponentHitOrigin	If true, uses the overlapping component's location for angle/distance checks instead of the actor pivot.
	 * @param IgnoredActors		Actors to exclude from the overlap results.
	 * @param ObjectTypes		Collision channels to query against.
	 * @return True if any actors were found within the wedge, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Wedge Shape overlap", meta=(Keywords = "Wedge, Shape, Overlap", DefaultToSelf = "WorldContext", WorldContext = "WorldContext"), Category = "ShapeOverlapLibrary")
	static bool OverlapWedgeShapeMulti(
		UObject* WorldContext, 
		TArray<AActor*>& OverlappedActors, 
		FTransform ParentTransform, 
		float Distance, 
		float Height, 
		float Angle,
		bool bUseOverlapComponentHitOrigin,
		TArray<AActor*> IgnoredActors,
		TArray<TEnumAsByte<ECollisionChannel>> ObjectTypes);
	
	//UFUNCTION(BlueprintCallable)
	static void DrawDebugWedge(		
		UObject* WorldContext, 
		FTransform ParentTransform, 
		float Distance, 
		float Height, 
		float Angle,
		bool bDrawBounds = true,
		FColor Color = FColor::Red,
		float Duration = 1);
	
	//UFUNCTION(BlueprintCallable)
	static bool OverlapSphereMulti(
		UObject* WorldContext,
		TArray<FOverlapResult>& OverlapResults,
		FVector Origin, 
		float Radius, 
		FCollisionQueryParams QueryParams,
		FCollisionObjectQueryParams ObjectQueryParams);
	
	//UFUNCTION(BlueprintCallable)
	static void DrawDebugOverlapSphere(
		UObject* WorldContext, 
		FVector Origin, 
		float Radius, 
		FColor Color = FColor::Red, 
		float Duration = 1);
};
