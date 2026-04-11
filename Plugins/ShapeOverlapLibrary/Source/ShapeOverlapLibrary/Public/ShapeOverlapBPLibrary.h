// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShapeOverlapBPLibrary.generated.h"

namespace ShapeLibraryCVars
{
	extern SHAPEOVERLAPLIBRARY_API TAutoConsoleVariable<bool> CVarDebugShapeOverlaps;
	extern SHAPEOVERLAPLIBRARY_API TAutoConsoleVariable<bool> CVarDebugShapeOverlapBounds;
	extern SHAPEOVERLAPLIBRARY_API TAutoConsoleVariable<bool> CVarDebugLineOfSight;
	extern SHAPEOVERLAPLIBRARY_API TAutoConsoleVariable<float> CVarDebugShapeOverlapDuration;
}

USTRUCT(BlueprintType, DisplayName="Hit Detection Info")
struct SHAPEOVERLAPLIBRARY_API FSLHitDetectionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bTestVisibility = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AActor*> IgnoredActors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TEnumAsByte<ECollisionChannel>> ObjectTypes = {ECC_Pawn, ECC_WorldDynamic};
};


UCLASS(DisplayName="Shape Overlap Library")
class SHAPEOVERLAPLIBRARY_API UShapeOverlapBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//Performs a very simple visibility line trace. Works well if none of the dynamic actors uses visibility blocking.
	UFUNCTION(BlueprintCallable, DisplayName="Line Of Sight To",
		meta=(DefaultToSelf = "WorldContext", WorldContext = "WorldContext", Keywords =
			"LOS, Line, Sight, Visiblity, Visible"), Category = "ShapeOverlapLibrary")
	static bool LineOfSightToPoint(const UObject* WorldContext, FVector Origin, FVector Target, const TArray<AActor*>& IgnoredActors, float Radius = 0);

	//More aligned for actor line of sight checks. Will add the actors to ignored for a result guaranteed to avoid invalid collision channels
	UFUNCTION(BlueprintCallable, Category = "ShapeOverlapLibrary")
	static bool LineOfSightToActor(const AActor* Actor, const AActor* Target, float Radius = 0.f, bool bUseTargetLocation = true);

	/**          
	 * Performs a wedge-shaped overlap check from the transform location in its forward direction.
	 * More expensive than a standard shape overlaps — use only when precise angular filtering is required.
	 * Visibility checks are included optionally through HitDetection info for optimized iterations.
	 *
	 * @param WorldContext		World context object (auto-bound to self in Blueprints).
	 * @param OverlappedActors	Output array of actors found within the wedge volume.
	 * @param ParentTransform	Origin and orientation of the wedge. Forward vector defines the facing direction.
	 * @param Distance		Radius of the wedge along the forward axis.
	 * @param Height		Total height of the wedge volume along the up axis.
	 * @param Angle			Full angular width of the wedge in degrees (e.g. 90 = 45 degrees either side of forward).
	 * @param InnerBiasRadius Inner radius to accept regardless of angles and LOS
	 * @param HitDetectionInfo Info containing actors to ignore and collision object types. Defaulted to Pawns and WorldDynamic.
	 * @return True if any actors were found within the wedge, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Wedge Shape overlap",
		meta=(Keywords = "Wedge, Shape, Overlap", DefaultToSelf = "WorldContext", WorldContext = "WorldContext"),
		Category = "ShapeOverlapLibrary")
	static bool OverlapWedgeShapeMulti(
		UObject* WorldContext,
		TArray<AActor*>& OverlappedActors,
		FTransform ParentTransform,
		float Distance,
		float Height,
		float Angle,
		FSLHitDetectionInfo HitDetectionInfo,
		float InnerBiasRadius = 0
	);

	static void DrawDebugWedge(
		UObject* WorldContext,
		FTransform ParentTransform,
		float Distance,
		float Height,
		float Angle,
		bool bDrawBounds = true,
		FColor Color = FColor::Red,
		float Duration = 1);
	
	/**          
	 * Performs a cheap sphere overlap that has optional LOS included.
	 * @param WorldContext		World context object (auto-bound to self in Blueprints).
	 * @param OverlappedActors	Output array of actors found within the sphere volume.
	 * @param ParentTransform	Transform
	 * @param TransformOffset   Offset using parent transform space
	 * @param Radius            Radius from the center of the Transform Origin + Transform offset
	 * @param HitDetectionInfo Info containing actors to ignore and collision object types. Defaulted to Pawns and WorldDynamic.
	 * @param bActorTargetLocationDistance  Instead of just accepting an overlap it also checks distance from the origin to the target location of the actor.
	 * @return True if any actors were found within the sphere, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Sphere Shape Overlap",
		meta=(Keywords = "Sphere, Shape, Overlap", DefaultToSelf = "WorldContext", WorldContext = "WorldContext"),
		Category = "ShapeOverlapLibrary")
	static bool OverlapSphereMulti(
		UObject* WorldContext,
		TArray<AActor*>& OverlappedActors,
		FTransform ParentTransform,
		FVector TransformOffset,
		float Radius,
		FSLHitDetectionInfo HitDetectionInfo,
		bool bActorTargetLocationDistance = false);
	
	UFUNCTION(BlueprintCallable, DisplayName="Box Shape Overlap",
	meta=(Keywords = "Box, Shape, Overlap", DefaultToSelf = "WorldContext", WorldContext = "WorldContext"),
	Category = "ShapeOverlapLibrary")
	static bool OverlapBoxShape(
		UObject* WorldContext, 
		TArray<AActor*>& OverlappedActors,
		FTransform ParentTransform, 
		FVector TransformOffset, 
		FVector Extents, 
		FSLHitDetectionInfo HitDetectionInfo);
	
	/**                   
	 * Performs a cylinder shaped overlap. 
	 * This can be achieved with edge overlap with 360 deg, but this is considerably cheaper.
	 * @param WorldContext		World context object (auto-bound to self in Blueprints).
	 * @param OverlappedActors	Output array of actors found within the sphere volume.
	 * @param ParentTransform	Transform
	 * @param TransformOffset   Offset using parent transform space
	 * @param Radius            Radius from the center of the Transform Origin + Transform offset
	 * @param HalfHeight        Half height from the center origin aka parent transform + offset, in the direction of the up axis of the parent transform.
	 * @param HitDetectionInfo Info containing actors to ignore and collision object types. Defaulted to Pawns and WorldDynamic.
	 * @return True if any actors were found within the sphere, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Cylinder Shape Overlap",
		meta=(Keywords = "Cylinder, Shape, Overlap", DefaultToSelf = "WorldContext", WorldContext = "WorldContext"),
		Category = "ShapeOverlapLibrary")
	static bool OverlapCylinderShape(
		UObject* WorldContext, 
		TArray<AActor*>& OverlappedActors,
		FTransform ParentTransform, 
		FVector TransformOffset, 
		float Radius,
		float HalfHeight,
		FSLHitDetectionInfo HitDetectionInfo);
};
