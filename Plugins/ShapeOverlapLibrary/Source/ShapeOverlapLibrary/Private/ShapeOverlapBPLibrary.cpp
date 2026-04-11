// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShapeOverlapBPLibrary.h"
#include "Engine/OverlapResult.h"
#include "Kismet/KismetSystemLibrary.h"

namespace ShapeLibraryCVars
{
	TAutoConsoleVariable<bool> CVarDebugShapeOverlaps(
	TEXT("ShapeLibrary.DrawDebug"),false,
	TEXT("Default = 0"),
	ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarDebugShapeOverlapBounds(
	TEXT("ShapeLibrary.DrawDebugBounds"),false,
	TEXT("Default = 0"),
	ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarDebugLineOfSight(
	TEXT("ShapeLibrary.DrawDebugLOS"),false,
	TEXT("Default = 0"),
	ECVF_Default);
	
	TAutoConsoleVariable<float> CVarDebugShapeOverlapDuration(
		TEXT("ShapeLibrary.DrawDebugDuration"),1,
		TEXT("Default = 1"),
		ECVF_Default);
}

bool UShapeOverlapBPLibrary::LineOfSightToPoint(const UObject* WorldContext, FVector Origin, FVector Target,
	const TArray<AActor*>& IgnoredActors, float Radius)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;
	
	FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam;
	Params.bTraceComplex = true;
	Params.AddIgnoredActors(IgnoredActors);
	
	bool bHasLOS;
	
	if (Radius > 0.0f) // Sphere check
	{
		const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
		
		FHitResult Result;
		bHasLOS = !UKismetSystemLibrary::SphereTraceSingle(
			World, 
			Origin, 
			Target, 
			Radius, 
			TraceType,
			true,
			IgnoredActors, 
			ShapeLibraryCVars::CVarDebugLineOfSight.GetValueOnGameThread() ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, 
			Result, 
			false,
			FLinearColor::Green,
			FLinearColor::Red,
			ShapeLibraryCVars::CVarDebugShapeOverlapDuration.GetValueOnGameThread());
	}
	else
	{
		bHasLOS = !World->LineTraceTestByChannel(Origin, Target, ECC_Visibility, Params);
		
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
	
		if (ShapeLibraryCVars::CVarDebugLineOfSight.GetValueOnGameThread())
		{
			DrawDebugDirectionalArrow(
				World, 
				Origin, 
				Target, 
				25, 
				bHasLOS ? FColor::Green : FColor::Yellow, 
				false, 
				ShapeLibraryCVars::CVarDebugShapeOverlapDuration.GetValueOnGameThread());
		}
	
#endif
	}
	
	return bHasLOS;
}

bool UShapeOverlapBPLibrary::LineOfSightToActor(const AActor* Actor, const AActor* Target, const float Radius, const bool bUseTargetLocation)
{
	if (!Actor || !Target) return false;
	
	const FVector Origin = bUseTargetLocation ? Actor->GetTargetLocation() : Actor->GetActorLocation();
	const FVector TargetLocation = bUseTargetLocation ? Target->GetTargetLocation() : Target->GetActorLocation();
	
	TArray<AActor*> IgnoreList;
	IgnoreList.Add(const_cast<AActor*>(Actor));
	IgnoreList.Add(const_cast<AActor*>(Target));
	return LineOfSightToPoint(Actor, Origin, TargetLocation, IgnoreList, Radius);;
}

bool UShapeOverlapBPLibrary::OverlapWedgeShapeMulti(
	UObject* WorldContext, 
	TArray<AActor*>& OverlappedActors, 
	FTransform ParentTransform, 
	float Distance, 
	float Height, 
	float Angle,
	FSLHitDetectionInfo HitDetectionInfo,
	float InnerBiasRadius)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;
	
    FQuat TargetRotation = ParentTransform.GetRotation();

	FVector ForwardDir = TargetRotation.GetForwardVector();
	FVector UpDir = TargetRotation.GetUpVector();
	
    float HalfHeight = Height * 0.5f * ParentTransform.GetScale3D().Z;
    FVector CenteredOrigin = ParentTransform.GetLocation();
	
	Distance *= ParentTransform.GetScale3D().X;
	
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
	
	if (ShapeLibraryCVars::CVarDebugShapeOverlaps.GetValueOnGameThread() || !World->IsGameWorld())
	{
		DrawDebugWedge(
			WorldContext, 
			ParentTransform, 
			Distance, 
			Height, 
			Angle, 
			ShapeLibraryCVars::CVarDebugShapeOverlapBounds.GetValueOnGameThread(), 
			FColor::Red, 
			ShapeLibraryCVars::CVarDebugShapeOverlapDuration.GetValueOnGameThread());
	}
	
#endif
	
    TArray<FOverlapResult> Overlaps;
	
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActors(HitDetectionInfo.IgnoredActors);
	
	FCollisionObjectQueryParams ObjectParams;
	for (auto Overlap : HitDetectionInfo.ObjectTypes)
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
	
	float ScaledInnerBias = InnerBiasRadius * ParentTransform.GetScale3D().X;
	
    for (const FOverlapResult& Overlap : Overlaps)
    {
    	if (OverlappedActors.Contains(Overlap.GetActor())) continue;
    	
    	FVector RefLocation = Overlap.GetActor()->GetTargetLocation();
    	
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
    	
    	float FlatDistSq = FlatToTarget.SizeSquared();
    	
    	if (FlatDistSq > Distance * Distance)
    	{
    		continue;
    	}
        
    	if (FlatToTarget.IsNearlyZero())
    	{
    		continue;
    	}

    	//Inside Bias Radius
	    if (FlatDistSq < FMath::Square(ScaledInnerBias))
    	{
    		FlatToTarget.Normalize();
    		FlatForward.Normalize();

    		if (float Dot = FVector::DotProduct(FlatToTarget, FlatForward); 
				Dot < CosHalfAngle)
    		{
    			continue;
    		}
	    	
	    	if (HitDetectionInfo.bTestVisibility && 
	    		!LineOfSightToPoint(WorldContext, CenteredOrigin, RefLocation, {Overlap.GetActor()})) continue;
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

bool UShapeOverlapBPLibrary::OverlapSphereMulti(UObject* WorldContext, TArray<AActor*>& OverlappedActors,
	FTransform ParentTransform, FVector TransformOffset, float Radius, FSLHitDetectionInfo HitDetectionInfo, bool bActorTargetLocationDistance)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;
	
	FVector Origin = ParentTransform.TransformPosition(TransformOffset);
	
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
	
	if (ShapeLibraryCVars::CVarDebugShapeOverlaps.GetValueOnGameThread() || !World->IsGameWorld())
	{
		DrawDebugSphere(
			World, 
			Origin, 
			Radius,
			32,
			FColor::Red,
			false,
			ShapeLibraryCVars::CVarDebugShapeOverlapDuration.GetValueOnGameThread());
	}
	
#endif
	
	TArray<FOverlapResult> Overlaps;
	
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActors(HitDetectionInfo.IgnoredActors);
	
	FCollisionObjectQueryParams ObjectParams;
	for (auto Overlap : HitDetectionInfo.ObjectTypes)
	{
		ObjectParams.AddObjectTypesToQuery(Overlap);
	}
	
	bool OverlappedAny = World->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(Radius),
		QueryParams);
	
	if (!OverlappedAny) return false;

	for (auto Overlap : Overlaps)
	{
		if (!Overlap.GetActor() || OverlappedActors.Contains(Overlap.GetActor())) continue;
		
		if (bActorTargetLocationDistance && 
			FVector::DistSquared(Overlap.GetActor()->GetTargetLocation(), Origin) > FMath::Square(Radius))
			continue;
		
		if (HitDetectionInfo.bTestVisibility && 
			!LineOfSightToPoint(WorldContext, Origin, Overlap.GetActor()->GetTargetLocation(), {Overlap.GetActor()})) continue;
		
		OverlappedActors.Add(Overlap.GetActor());
	}
	
	return true;
}

bool UShapeOverlapBPLibrary::OverlapBoxShape(UObject* WorldContext, TArray<AActor*>& OverlappedActors, FTransform ParentTransform, FVector TransformOffset,
	FVector Extents, FSLHitDetectionInfo HitDetectionInfo)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;
	
	FVector Origin = ParentTransform.TransformPosition(TransformOffset);
	
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
	
	if (ShapeLibraryCVars::CVarDebugShapeOverlaps.GetValueOnGameThread() || !World->IsGameWorld())
	{
		DrawDebugBox(
			World, 
			Origin, 
			Extents * ParentTransform.GetScale3D(), 
			ParentTransform.GetRotation(), 
			FColor::Red, 
			false, 
			ShapeLibraryCVars::CVarDebugShapeOverlapDuration.GetValueOnGameThread());
	}
	
#endif
	
	TArray<FOverlapResult> Overlaps;
	
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActors(HitDetectionInfo.IgnoredActors);
	
	FCollisionObjectQueryParams ObjectParams;
	for (auto Overlap : HitDetectionInfo.ObjectTypes)
	{
		ObjectParams.AddObjectTypesToQuery(Overlap);
	}
	
	bool OverlappedAny = World->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		ParentTransform.GetRotation(),
		ObjectParams,
		FCollisionShape::MakeBox(Extents * ParentTransform.GetScale3D()),
		QueryParams);
	
	if (!OverlappedAny) return false;

	for (auto Overlap : Overlaps)
	{
		if (!Overlap.GetActor() || OverlappedActors.Contains(Overlap.GetActor())) continue;
		
		if (HitDetectionInfo.bTestVisibility && 
			!LineOfSightToPoint(WorldContext, Origin, Overlap.GetActor()->GetTargetLocation(), {Overlap.GetActor()})) continue;
		
		OverlappedActors.Add(Overlap.GetActor());
	}
	
	return true;
}

bool UShapeOverlapBPLibrary::OverlapCylinderShape(UObject* WorldContext, TArray<AActor*>& OverlappedActors,
	FTransform ParentTransform, FVector TransformOffset, float Radius, float HalfHeight,
	FSLHitDetectionInfo HitDetectionInfo)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;

	FVector Origin = ParentTransform.TransformPosition(TransformOffset);
	FVector UpVector = ParentTransform.GetUnitAxis(EAxis::Z);
	HalfHeight *= ParentTransform.GetScale3D().Z;
	float ScaledRadius = Radius * FMath::Max(ParentTransform.GetScale3D().X, ParentTransform.GetScale3D().Y);

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT

	if (ShapeLibraryCVars::CVarDebugShapeOverlaps.GetValueOnGameThread() || !World->IsGameWorld())
	{
		DrawDebugCylinder(
			World,
			Origin - UpVector * HalfHeight,
			Origin + UpVector * HalfHeight,
			ScaledRadius,
			32, FColor::Red,
			false,
			ShapeLibraryCVars::CVarDebugShapeOverlapDuration.GetValueOnGameThread());
		
		if (ShapeLibraryCVars::CVarDebugShapeOverlapBounds.GetValueOnGameThread())
		{
			DrawDebugBox(
				World, 
				Origin, 
				FVector(ScaledRadius, ScaledRadius, HalfHeight),
				ParentTransform.GetRotation(),
				FColor::Red, 
				false, 
				ShapeLibraryCVars::CVarDebugShapeOverlapDuration.GetValueOnGameThread());
		}
	}

#endif

	TArray<FOverlapResult> Overlaps;

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActors(HitDetectionInfo.IgnoredActors);

	FCollisionObjectQueryParams ObjectParams;
	for (auto Overlap : HitDetectionInfo.ObjectTypes)
	{
		ObjectParams.AddObjectTypesToQuery(Overlap);
	}

	bool OverlappedAny = World->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		ParentTransform.GetRotation(),
		ObjectParams,
		FCollisionShape::MakeBox(FVector(ScaledRadius, ScaledRadius, HalfHeight)),
		QueryParams);

	if (!OverlappedAny) return false;

	for (auto& Overlap : Overlaps)
	{
		if (!Overlap.GetActor() || OverlappedActors.Contains(Overlap.GetActor())) continue;
		FVector ToActor = Overlap.GetActor()->GetTargetLocation() - Origin;
		
		float UpDistance = FVector::DotProduct(ToActor, UpVector);   
		if (FMath::Abs(UpDistance) > HalfHeight) continue; //Out of bounds of the top of cylinder
		
		FVector Radial = ToActor - UpVector * UpDistance;
		if (Radial.SizeSquared() > ScaledRadius * ScaledRadius) continue; //Outside the radius of the cylinder
		
		OverlappedActors.Add(Overlap.GetActor());
	}

	return true;
}

