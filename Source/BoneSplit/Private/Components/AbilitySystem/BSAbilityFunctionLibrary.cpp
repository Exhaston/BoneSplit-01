// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/BSAbilityFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Engine/OverlapResult.h"

TArray<AActor*> UBSAbilityFunctionLibrary::SliceShapeOverlap(
UObject* WorldContext, FTransform Transform, TEnumAsByte<EAxis::Type> ForwardAxis,
		TEnumAsByte<EAxis::Type> UpAxis, float Angle, float Height, float Distance, 
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels)
{
 TArray<AActor*> Result;
    if (!WorldContext) return Result;

    UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	
    FQuat TargetRotation = Transform.GetRotation();

	FVector ForwardDir = GetSwizzledDirectionFromRotation(ForwardAxis, TargetRotation);
	FVector UpDir = GetSwizzledDirectionFromRotation(UpAxis, TargetRotation);
	
    float HalfHeight = Height * 0.5f;
    FVector CenteredOrigin = Transform.GetLocation();
	
    FVector BoxExtent(Distance, Distance, HalfHeight);
    FCollisionShape Box = FCollisionShape::MakeBox(BoxExtent);
	
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
        Box,
        Params)) 
    	return Result;

    const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(Angle * 0.5f));
	
    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* Actor = Overlap.GetActor();
        if (!Actor) continue;
    	
        FVector ToTarget = Overlap.GetActor()->GetActorLocation() - CenteredOrigin;

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
    	
    	float Dot = FVector::DotProduct(FlatToTarget, FlatForward);
    	if (Dot < CosHalfAngle)
    	{
    		continue;
    	}
    	
    	DrawDebugPoint(
    		World, 
    		Overlap.GetActor()->GetActorLocation(), 
    		15, 
    		FColor::Red, 
    		false, 
    		1);

        Result.AddUnique(Actor);
    }

    return Result;
}

void UBSAbilityFunctionLibrary::DrawDebugSlice(
	const UObject* WorldContext,
	FTransform Transform,
	TEnumAsByte<EAxis::Type> ForwardAxis,
	TEnumAsByte<EAxis::Type> UpAxis,
	const float Radius,
	const float Height,
	const float AngleDegrees,
	const int32 ArcSegments,
	const FColor Color,
	const float Duration,
	const float Thickness)
{
    if (!WorldContext) return;
	
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	
	DrawDebugCoordinateSystem(World, Transform.GetLocation(), Transform.GetRotation().Rotator(), Radius * 0.5, false, Duration, 0, Thickness);
	
	const FVector ForwardDir = GetSwizzledDirectionFromRotation(ForwardAxis, Transform.GetRotation());
	
	FVector UpDir = GetSwizzledDirectionFromRotation(UpAxis, Transform.GetRotation());
	
    const float HalfHeight = Height * 0.5f;

    const FVector TopOrigin = Transform.GetLocation() + UpDir * HalfHeight;
    const FVector BottomOrigin = Transform.GetLocation() - UpDir * HalfHeight;

	const bool IsCircle = AngleDegrees >= 360 || AngleDegrees <= -360;
	
    const float HalfAngle = AngleDegrees * 0.5f;
	
	DrawDebugBox(
	World,
	Transform.GetLocation(),
	FVector(Radius, Radius, HalfHeight),
	FRotationMatrix::MakeFromXZ(ForwardDir, UpDir).ToQuat(),
	Color,
	false,
	Duration,
	0,
	Thickness
	);
	
    const FVector LeftDir =
        ForwardDir.RotateAngleAxis(-HalfAngle, UpDir);
	
	const FVector RightDir =
	ForwardDir.RotateAngleAxis(HalfAngle, UpDir);

	//Draw Cone slice
    auto DrawSlice = [&](const FVector& SliceOrigin)
    {
    	if (!IsCircle)
    	{
    		DrawDebugLine(
    			World,
    			SliceOrigin,
    			SliceOrigin + LeftDir * Radius,
    			Color,
    			false,
    			Duration,
    			0,
    			Thickness);

    		DrawDebugLine(
				World,
				SliceOrigin,
				SliceOrigin + RightDir * Radius,
				Color,
				false,
				Duration,
				0,
				Thickness);
    	}
    	
        FVector PrevPoint = SliceOrigin + LeftDir * Radius;

        for (int32 i = 1; i <= ArcSegments; ++i)
        {
            const float Alpha = static_cast<float>(i) / ArcSegments;
            const float Angle = -HalfAngle + AngleDegrees * Alpha;

            FVector Dir =
                ForwardDir.RotateAngleAxis(Angle, UpDir);

            FVector Point = SliceOrigin + Dir * Radius;

            DrawDebugLine(
                World,
                PrevPoint,
                Point,
                Color,
                false,
                Duration,
                0,
                Thickness
            );

            PrevPoint = Point;
        }
    };
	
    DrawSlice(TopOrigin);                       
    DrawSlice(BottomOrigin);

    //Vertical lines
    for (int32 i = 0; i <= ArcSegments; ++i)
    {
        const float Alpha = static_cast<float>(i) / ArcSegments;
        const float Angle = -HalfAngle + AngleDegrees * Alpha;

        FVector Dir =
            ForwardDir.RotateAngleAxis(Angle, UpDir);

        FVector BottomPoint = BottomOrigin + Dir * Radius;
        FVector TopPoint = TopOrigin + Dir * Radius;

        DrawDebugLine(
            World,
            BottomPoint,
            TopPoint,
            Color,
            false,
            Duration,
            0,
            Thickness
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
			Thickness);
		
		DrawDebugLine(
			World,
			BottomOrigin + LeftDir * Radius,
			TopOrigin + LeftDir * Radius,
			Color,
			false,
			Duration,
			0,
			Thickness);
	
		DrawDebugLine(
			World,
			BottomOrigin + RightDir * Radius,
			TopOrigin + RightDir * Radius,
			Color,
			false,
			Duration,
			0,
			Thickness);
	}
}

FVector UBSAbilityFunctionLibrary::GetSwizzledDirectionFromRotation(const TEnumAsByte<EAxis::Type> InAxis,
                                                                    const FQuat& InRotation)
{
	switch (InAxis)
	{
		case EAxis::X:
		return InRotation.GetForwardVector();
		
		case EAxis::Y:
		return InRotation.GetRightVector();
		
		case EAxis::Z:
		return InRotation.GetUpVector();
		
		default:
		return InRotation.GetForwardVector();
	}
}

UBSAbilitySystemComponent* UBSAbilityFunctionLibrary::GetBSAbilitySystemComponent(AActor* ActorToTest)
{
	if (ActorToTest)
	{
		if (UBSAbilitySystemComponent* Asc = Cast<UBSAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToTest)))
		{
			return Asc;
		}	
	}
	return nullptr;
}
