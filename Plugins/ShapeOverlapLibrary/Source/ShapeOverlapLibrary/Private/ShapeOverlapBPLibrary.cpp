// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShapeOverlapBPLibrary.h"
#include "Engine/OverlapResult.h"

bool UShapeOverlapBPLibrary::LineOfSightTo(UObject* WorldContext, const FVector Origin, const FVector Target)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;
	
	FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam;
	Params.bTraceComplex = true;
	return World->LineTraceTestByChannel(Origin, Target, ECC_Visibility, Params);
}

bool UShapeOverlapBPLibrary::OverlapWedgeShapeMulti(
	UObject* WorldContext, 
	TArray<AActor*>& OverlappedActors, 
	FTransform ParentTransform, 
	float Distance, 
	float Height, 
	float Angle,
	bool bUseOverlapComponentHitOrigin,
	TArray<AActor*> IgnoredActors,
	TArray<TEnumAsByte<ECollisionChannel>> ObjectTypes)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;
	
    FQuat TargetRotation = ParentTransform.GetRotation();

	FVector ForwardDir = TargetRotation.GetForwardVector();
	FVector UpDir = TargetRotation.GetUpVector();
	
    float HalfHeight = Height * 0.5f * ParentTransform.GetScale3D().Z;
    FVector CenteredOrigin = ParentTransform.GetLocation();
	
	Distance *= ParentTransform.GetScale3D().X;
	
    TArray<FOverlapResult> Overlaps;
	
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActors(IgnoredActors);
	
	FCollisionObjectQueryParams ObjectParams;
	for (auto Overlap : ObjectTypes)
	{
		ObjectParams.AddObjectTypesToQuery(Overlap);
	}
	
	bool OverlappedAny = World->OverlapMultiByObjectType(
		Overlaps,
		CenteredOrigin,
		FRotationMatrix::MakeFromXZ(ForwardDir, UpDir).ToQuat(),
		ObjectParams,
		FCollisionShape::MakeBox(FVector{Distance, Distance, HalfHeight}),
		QueryParams);
	
	if (!OverlappedAny) return false;

    const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(Angle * 0.5f));
	
    for (const FOverlapResult& Overlap : Overlaps)
    {
    	if (OverlappedActors.Contains(Overlap.GetActor())) continue;
    	
    	FVector RefLocation = bUseOverlapComponentHitOrigin ?
    	Overlap.GetComponent()->GetComponentLocation() : Overlap.GetActor()->GetActorLocation();
    	
        FVector ToTarget = RefLocation - CenteredOrigin;

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
    	
        OverlappedActors.Add(Overlap.GetActor());
    }

    return !OverlappedActors.IsEmpty();
}

void UShapeOverlapBPLibrary::DrawDebugWedge(UObject* WorldContext,
FTransform ParentTransform, float Distance, float Height, float Angle, bool bDrawBounds, FColor Color,
float Duration)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
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

bool UShapeOverlapBPLibrary::OverlapSphereMulti(
	UObject* WorldContext, 
	TArray<FOverlapResult>& OverlapResults, 
	const FVector Origin, 
	float Radius,
	const FCollisionQueryParams QueryParams, 
	const FCollisionObjectQueryParams ObjectQueryParams)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;
	
	Radius = FMath::Clamp(Radius, 0.f, FLT_MAX);
	
	return World->OverlapMultiByObjectType(
		OverlapResults, 
		Origin, 
		FQuat::Identity, 
		ObjectQueryParams, 
		FCollisionShape::MakeSphere(Radius), QueryParams);
}

void UShapeOverlapBPLibrary::DrawDebugOverlapSphere(
	UObject* WorldContext, 
	const FVector Origin, 
	float Radius, 
	const FColor Color,
	const float Duration)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return;
	
	Radius = FMath::Clamp(Radius, 0.f, FLT_MAX);
	
	DrawDebugSphere(World, Origin, Radius, 16, Color, false, Duration);
}

