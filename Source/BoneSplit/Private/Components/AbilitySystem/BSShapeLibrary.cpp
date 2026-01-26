// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/BSShapeLibrary.h"

#include "Engine/OverlapResult.h"

TArray<AActor*> UBSShapeLibrary::BP_OverlapShapeWedge(
	UObject* WorldContext, 
	const FTransform ParentTransform, 
	const float Distance,
	const float Height, 
	const float Angle,
	const TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels,
	const bool bCheckVisibility)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	return OverlapShapeWedge(World, ParentTransform, Distance, Height, Angle, CollisionChannels, bCheckVisibility);
}

TArray<AActor*> UBSShapeLibrary::OverlapShapeWedge(
	UWorld* World, 
	FTransform ParentTransform, 
	float Distance,
	float Height, 
	float Angle, 
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels,
	bool bCheckVisibility)
{
	TArray<AActor*> Result;
    if (!World) return Result;
	
    FQuat TargetRotation = ParentTransform.GetRotation();

	FVector ForwardDir = TargetRotation.GetForwardVector();
	FVector UpDir = TargetRotation.GetUpVector();
	
    float HalfHeight = Height * 0.5f * ParentTransform.GetScale3D().Z;
    FVector CenteredOrigin = ParentTransform.GetLocation();
	
	Distance *= ParentTransform.GetScale3D().Y;
	
	if (!World->IsGameWorld()) return Result;
	
    FCollisionQueryParams Params;
    Params.bTraceComplex = false;

    FCollisionObjectQueryParams QueryParams;
    for (auto Channel : CollisionChannels)
        QueryParams.AddObjectTypesToQuery(Channel);
	
    TArray<FOverlapResult> Overlaps;
	
    if (!World->OverlapMultiByObjectType(
        Overlaps,
        CenteredOrigin,
        FRotationMatrix::MakeFromXZ(ForwardDir, UpDir).ToQuat(),
        QueryParams,
        FCollisionShape::MakeBox(FVector{Distance, Distance, HalfHeight}),
        Params)) 
    	return Result;

    const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(Angle * 0.5f));
	
    for (const FOverlapResult& Overlap : Overlaps)
    {
    	//Avoid checks if we already have the overlap actor in the array
    	if (Result.Contains(Overlap.GetActor())) continue; 
    	
        AActor* Actor = Overlap.GetActor();                                                        
        if (!Actor) continue;
    	
    	if (bCheckVisibility)
    	{
    		if (!IsVisible(
				World, 
				CenteredOrigin, 
				Actor->GetActorLocation()))
    		{
    			continue;
    		}
    	}
    	
        FVector ToTarget = Actor->GetActorLocation() - CenteredOrigin;

        if (float DistanceAlongUp = FVector::DotProduct(ToTarget, UpDir); 
        	FMath::Abs(DistanceAlongUp) > HalfHeight)
        {
        	continue;
        } 
    	
        if (ToTarget.IsNearlyZero())
        {
        	continue;
        }
    	
    	FVector FlatToTarget = ToTarget - FVector::DotProduct(ToTarget, UpDir) * UpDir;
    	FVector FlatForward  = ForwardDir - FVector::DotProduct(ForwardDir, UpDir) * UpDir;
    	
    	if (FlatToTarget.SizeSquared() > Distance * Distance)
    	{
    		continue;
    	}
    	
    	if (FlatToTarget.IsNearlyZero())
    	{
    		continue;
    	}

    	FlatToTarget.Normalize();
    	FlatForward.Normalize();

        if (float Dot = FVector::DotProduct(FlatToTarget, FlatForward); 
        	Dot < CosHalfAngle)
    	{
    		continue;
    	}
    	
        Result.Add(Overlap.GetActor());
    }

    return Result;
}

void UBSShapeLibrary::BP_DebugDrawWedge(
	UObject* WorldContext,
	const FTransform ParentTransform,
	const float Distance,
	const float Height,
	const float Angle,
	const float Duration,
	const FColor Color,
	const bool bDrawBounds)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	DebugDrawWedge(World, ParentTransform, Distance, Height, Angle, Duration, Color, bDrawBounds);
}

void UBSShapeLibrary::DebugDrawWedge(
	UWorld* World, 
	FTransform ParentTransform, 
	float Distance, 
	float Height, 
	float Angle, 
	float Duration, 
	FColor Color,
	bool bDrawBounds)
{
	 if (!World) return;
	
	DrawDebugCoordinateSystem(
		World, 
		ParentTransform.GetLocation(),
		ParentTransform.GetRotation().Rotator(), 
		Distance * 0.5, 
		false, 
		Duration, 
		0, 
		0);
	
	Distance *= ParentTransform.GetScale3D().Y;
	Height *= ParentTransform.GetScale3D().Z;
	
	const FVector ForwardDir = ParentTransform.GetRotation().GetForwardVector();
	
	FVector UpDir = ParentTransform.GetRotation().GetUpVector();
	
    const float HalfHeight = Height * 0.5f;

    const FVector TopOrigin = ParentTransform.GetLocation() + UpDir * HalfHeight;
    const FVector BottomOrigin = ParentTransform.GetLocation() - UpDir * HalfHeight;

	const bool IsCircle = Angle >= 360 || Angle <= -360;
	
    const float HalfAngle = Angle * 0.5f;
	
	if (bDrawBounds)
	{
		DrawDebugBox(
			World,
			ParentTransform.GetLocation(),
			FVector(Distance, Distance, HalfHeight),
			FRotationMatrix::MakeFromXZ(ForwardDir, UpDir).ToQuat(),
			Color,
			false,
			Duration,
			0,
			0
			);
	}
	
    const FVector LeftDir =
        ForwardDir.RotateAngleAxis(-HalfAngle, UpDir);
	
	const FVector RightDir =
	ForwardDir.RotateAngleAxis(HalfAngle, UpDir);
	
	int32 Segments = 16;

	//Draw Cone slice
    auto DrawSlice = [&](const FVector& SliceOrigin)
    {
    	if (!IsCircle)
    	{
    		DrawDebugLine(
    			World,
    			SliceOrigin,
    			SliceOrigin + LeftDir * Distance,
    			Color,
    			false,
    			Duration,
    			0,
    			0);

    		DrawDebugLine(
				World,
				SliceOrigin,
				SliceOrigin + RightDir * Distance,
				Color,
				false,
				Duration,
				0,
				0);
    	}
    	
        FVector PrevPoint = SliceOrigin + LeftDir * Distance;
    	
        for (int32 i = 1; i <= Segments; ++i)
        {
            const float Alpha = static_cast<float>(i) / Segments;
            const float CurrentAngle = -HalfAngle + Angle * Alpha;

            FVector Dir =
                ForwardDir.RotateAngleAxis(CurrentAngle, UpDir);

            FVector Point = SliceOrigin + Dir * Distance;

            DrawDebugLine(
                World,
                PrevPoint,
                Point,
                Color,
                false,
                Duration,
                0,
                0
            );

            PrevPoint = Point;
        }
    };
	
    DrawSlice(TopOrigin);                       
    DrawSlice(BottomOrigin);

    //Vertical lines
    for (int32 i = 0; i <= Segments; ++i)
    {
        const float Alpha = static_cast<float>(i) / Segments;
        const float CurrentAngle = -HalfAngle + Angle * Alpha;

        FVector Dir =
            ForwardDir.RotateAngleAxis(CurrentAngle, UpDir);

        FVector BottomPoint = BottomOrigin + Dir * Distance;
        FVector TopPoint = TopOrigin + Dir * Distance;

        DrawDebugLine(
            World,
            BottomPoint,
            TopPoint,
            Color,
            false,
            Duration,
            0,
            0
        );
    }
	
	if (!IsCircle)
	{	
		DrawDebugLine(
			World,
			BottomOrigin,
			TopOrigin,
			Color,
			false,
			Duration,
			0,
			0);
		
		DrawDebugLine(
			World,
			BottomOrigin + LeftDir * Distance,
			TopOrigin + LeftDir * Distance,
			Color,
			false,
			Duration,
			0,
			0);
	
		DrawDebugLine(
			World,
			BottomOrigin + RightDir * Distance,
			TopOrigin + RightDir * Distance,
			Color,
			false,
			Duration,
			0,
			0);
	}
}

TArray<AActor*> UBSShapeLibrary::BP_SphereOverlapActors(
	UObject* WorldContext,
	const FTransform ParentTransform,
	const float Radius,
	const TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels,
	const bool bCheckVisibility)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	return SphereOverlapActors(World, ParentTransform, Radius, CollisionChannels, bCheckVisibility);
}

TArray<AActor*> UBSShapeLibrary::SphereOverlapActors(
	const UWorld* World,
	const FTransform& ParentTransform, 
	const float Radius,
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels,
	const bool bCheckVisibility)
{
	TArray<AActor*> Results;
	
	if (!World) return Results;
	
	FCollisionObjectQueryParams QueryParams;
	
	for (auto CollisionChannel : CollisionChannels)
		QueryParams.AddObjectTypesToQuery(CollisionChannel);
	
	TArray<AActor*> UniqueHitActors;
	
	TArray<FOverlapResult> Overlaps;
	
	World->OverlapMultiByObjectType(
		Overlaps, 
		ParentTransform.GetLocation(), 
		FQuat::Identity, 
		QueryParams, 
		FCollisionShape::MakeSphere(Radius * ParentTransform.GetScale3D().X));
	
	for (auto OverlapResult : Overlaps)
	{
		if (OverlapResult.GetActor())
		{
			if (bCheckVisibility)
			{
				if (!IsVisible(
					World, 
					ParentTransform.GetLocation(), 
					OverlapResult.GetActor()->GetActorLocation()))
				{
					continue;
				}
			}
			Results.AddUnique(OverlapResult.GetActor());
		}
	}
	return Results;
}

bool UBSShapeLibrary::BP_IsVisible(UObject* WorldContext, const FVector OriginPoint, const FVector TargetPoint)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	return IsVisible(World, OriginPoint, TargetPoint);
}

bool UBSShapeLibrary::IsVisible(const UWorld* World, const FVector& OriginPoint, const FVector& TargetPoint)
{
	FHitResult HitResult;
	
	return !World->LineTraceSingleByChannel(
		HitResult, 
		OriginPoint, 
		TargetPoint, 
		ECC_Visibility);
}
