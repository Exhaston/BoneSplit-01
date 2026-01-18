// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/BSAbilityFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Actors/Mob/BSTargetSetting.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/Targeting/BSThreatComponent.h"
#include "Components/Targeting/BSThreatInterface.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tasks/AITask_MoveTo.h"
#include "Components/AbilitySystem/EffectBases/BSGameplayEffect.h"

bool UBSAbilityFunctionLibrary::FilterByAnyMatchingFactions(UAbilitySystemComponent* Asc, UAbilitySystemComponent* OtherAsc)
{
	FGameplayTagContainer FilterContainer;
	FilterContainer.AddTagFast(BSTags::Faction);

	const FGameplayTagContainer FilteredOwnerTags = Asc->GetOwnedGameplayTags().Filter(FilterContainer);
	
	if (const FGameplayTagContainer FilteredOtherCompTags = 
		OtherAsc->GetOwnedGameplayTags().Filter(FilterContainer); 
		FilteredOtherCompTags.HasAnyExact(FilteredOwnerTags))
	{
		return true;
	}
	return false;
}

bool UBSAbilityFunctionLibrary::FilterByNoMatchingFactions(UAbilitySystemComponent* Asc, UAbilitySystemComponent* OtherAsc)
{
	FGameplayTagContainer FilterContainer;
	FilterContainer.AddTagFast(BSTags::Faction);

	const FGameplayTagContainer FilteredOwnerTags = Asc->GetOwnedGameplayTags().Filter(FilterContainer);
	
	if (const FGameplayTagContainer FilteredOtherCompTags = 
		OtherAsc->GetOwnedGameplayTags().Filter(FilterContainer); 
		!FilteredOtherCompTags.HasAnyExact(FilteredOwnerTags))
	{
		return true;
	}
	return false;
}

void UBSAbilityFunctionLibrary::TryActivateAbilityWeighted(UAbilitySystemComponent* OwnerAsc,
                                                           const TMap<TSubclassOf<UGameplayAbility>, float> AbilityMap)
{
	if (const TSubclassOf<UGameplayAbility> ChosenAbility = GetAbilityByWeight(OwnerAsc, AbilityMap))
	{
		OwnerAsc->TryActivateAbilityByClass(ChosenAbility);
	}
}

void UBSAbilityFunctionLibrary::LazyApplyEffectToTarget(UGameplayAbility* Ability,
                                                        	AActor* TargetActor, const TSubclassOf<UBSGameplayEffect> EffectClass, const FVector Origin, FBSTargetFilter Filter, const float EffectLevel)
{
	UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	if (!TargetAsc || !EffectClass) return;
	
	UAbilitySystemComponent* SourceAsc = Ability->GetAbilitySystemComponentFromActorInfo();
	
	if (!Filter.RunFilter(SourceAsc->GetAvatarActor(), TargetActor))
	{
		return;
	}
	
	check(SourceAsc);
	FGameplayEffectContextHandle ContextHandle = SourceAsc->MakeEffectContext();
	ContextHandle.AddOrigin(Origin);
	ContextHandle.SetAbility(Ability);

	const FGameplayEffectSpecHandle Handle = 
		SourceAsc->MakeOutgoingSpec(EffectClass, EffectLevel, ContextHandle);
	
	SourceAsc->ApplyGameplayEffectSpecToTarget(*Handle.Data, TargetAsc);
}

bool UBSAbilityFunctionLibrary::CanMobReachLocation(AActor* InMob, const FVector TargetLocation)
{
	if (!InMob) return false;

	const AAIController* AIController = UAIBlueprintHelperLibrary::GetAIController(InMob);
	if (!AIController) return false;

	UNavigationSystemV1* NavSys =
		UNavigationSystemV1::GetCurrent(InMob->GetWorld());

	if (!NavSys)
	{
		return false;
	}

	const ANavigationData* NavData =
		NavSys->GetNavDataForProps(AIController->GetNavAgentPropertiesRef());

	if (!NavData)
	{
		return false;
	}

	const FPathFindingQuery Query = FPathFindingQuery(
		AIController,
		*NavData,
		InMob->GetActorLocation(),
		TargetLocation
	);

	const FPathFindingResult Result = NavSys->FindPathSync(Query);

	return Result.IsSuccessful();
}

bool UBSAbilityFunctionLibrary::CheckMobGrounded(AActor* InMob)
{
	if (const ACharacter* MobAsCharacter = Cast<ACharacter>(InMob))
	{
		return MobAsCharacter->GetCharacterMovement()->IsMovingOnGround();
	}
	return false;
}

TArray<AActor*> UBSAbilityFunctionLibrary::FilterArrayByReachable(AActor* InOriginActor, TArray<AActor*> InActors)
{
	TArray<AActor*> OutActors;
	for (auto Actor : InActors)
	{
		if (CanMobReachLocation(InOriginActor, Actor->GetActorLocation()))
		{
			 OutActors.Add(Actor);
		}
	}
	return OutActors;
}

TArray<AActor*> UBSAbilityFunctionLibrary::FilterArrayByVisible(
	UObject* WorldContext, 
	const FVector InOrigin, 
	TArray<AActor*> InActors)
{
	TArray<AActor*> OutActors;
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	for (auto Actor : InActors)
	{
		if (FHitResult HitResult; 
			!World->LineTraceSingleByChannel(
			HitResult, 
			InOrigin,
			Actor->GetActorLocation(), 
			ECC_Visibility))
		{
			OutActors.AddUnique(Actor);
		}
	}
	return OutActors;
}

bool UBSAbilityFunctionLibrary::CheckTargetVisibility(UObject* WorldContext, const FVector InOrigin, const AActor* InTargetActor)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	FHitResult HitResult; 
	
	return !World->LineTraceSingleByChannel(
		HitResult, 
		InOrigin,
		InTargetActor->GetActorLocation(), 
		ECC_Visibility);
}

TArray<AActor*> UBSAbilityFunctionLibrary::GetActorsInRadius(
	AActor* InOriginActor, const float InRange, bool bIncludeSelf)
{
	TArray<AActor*> OutActors;
	if (!InOriginActor)
	{
		return OutActors;
	}

	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(InOriginActor);

	const FVector OriginLocation = InOriginActor->GetActorLocation();
	FCollisionShape Sphere = FCollisionShape::MakeSphere(InRange);
	
	TArray<FOverlapResult> Overlaps;
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	
	FCollisionQueryParams QueryParams;
	
	if (!bIncludeSelf)
	{
		QueryParams.AddIgnoredActor(InOriginActor);
	}
	
	World->OverlapMultiByObjectType(
		Overlaps, OriginLocation, FQuat::Identity, ObjectQueryParams, Sphere, QueryParams);

	for (auto OverlapResult : Overlaps)
	{
		if (AActor* OverlappedActor = OverlapResult.GetActor(); 
			OverlappedActor && !OverlappedActor->IsPendingKillPending())
		{
			OutActors.AddUnique(OverlappedActor);
		}
	}

	return OutActors;
}

AActor* UBSAbilityFunctionLibrary::PickRandomTargetFromArray(TArray<AActor*> InActors)
{
	if (InActors.IsEmpty())
	{
		return nullptr;
	}

	const int32 RandomIndex = FMath::RandRange(0, InActors.Num() - 1);
	return InActors[RandomIndex];
}

AActor* UBSAbilityFunctionLibrary::PickClosestTargetFromArray(AActor* InOriginActor, TArray<AActor*> InActors)
{
	if (!InOriginActor || InActors.IsEmpty())
	{
		return nullptr;
	}

	AActor* ClosestActor = nullptr;
	float ClosestDistanceSq = FLT_MAX;

	const FVector OriginLocation = InOriginActor->GetActorLocation();

	for (AActor* Actor : InActors)
	{
		if (!Actor || Actor == InOriginActor)
		{
			continue;
		}

		const float DistanceSq =
			FVector::DistSquared(OriginLocation, Actor->GetActorLocation());

		if (DistanceSq < ClosestDistanceSq)
		{
			ClosestDistanceSq = DistanceSq;
			ClosestActor = Actor;
		}
	}

	return ClosestActor;
}

AActor* UBSAbilityFunctionLibrary::PickFarthestTargetFromArray(AActor* InOriginActor, TArray<AActor*> InActors)
{
	if (!InOriginActor || InActors.IsEmpty())
	{
		return nullptr;
	}

	AActor* FarthestActor = nullptr;
	float FarthestDistanceSq = 0.f;

	const FVector OriginLocation = InOriginActor->GetActorLocation();

	for (AActor* Actor : InActors)
	{
		if (!Actor || Actor == InOriginActor)
		{
			continue;
		}

		const float DistanceSq =
			FVector::DistSquared(OriginLocation, Actor->GetActorLocation());

		if (DistanceSq > FarthestDistanceSq)
		{
			FarthestDistanceSq = DistanceSq;
			FarthestActor = Actor;
		}
	}

	return FarthestActor;
}

TArray<AActor*> UBSAbilityFunctionLibrary::GetThreatActors(AActor* InTarget)
{
	if (UBSThreatComponent* ThreatComponent = GetThreatComponent(InTarget))
	{
		return ThreatComponent->GetThreatActors();
	}
	return {};
}

AActor* UBSAbilityFunctionLibrary::GetHighestThreatActor(AActor* InTarget)
{
	if (UBSThreatComponent* ThreatComponent = GetThreatComponent(InTarget))
	{
		return ThreatComponent->GetHighestThreatActor();
	}
	return nullptr;
}

AActor* UBSAbilityFunctionLibrary::GetLowestThreatActor(AActor* InTarget)
{
	if (UBSThreatComponent* ThreatComponent = GetThreatComponent(InTarget))
	{
		return ThreatComponent->GetLowestThreatActor();
	}
	return nullptr;
}

UBSThreatComponent* UBSAbilityFunctionLibrary::GetThreatComponent(AActor* InTarget)
{
	if (IBSThreatInterface* ThreatInterface = Cast<IBSThreatInterface>(InTarget))
	{
		 return ThreatInterface->GetThreatComponent();             
	}
	return InTarget->GetComponentByClass<UBSThreatComponent>(); //Fallback, can return null
}

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
	
    float HalfHeight = Height * 0.5f * Transform.GetScale3D().Z;
    FVector CenteredOrigin = Transform.GetLocation();
	
	Distance *= Transform.GetScale3D().Y;
	
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

        if (float Dot = FVector::DotProduct(FlatToTarget, FlatForward); 
        	Dot < CosHalfAngle)
    	{
    		continue;
    	}
    	
    	if (BSConsoleVariables::CVarBSDebugHitDetection.GetValueOnGameThread())
    	{
    		DrawDebugPoint(
    		World, 
    		Overlap.GetActor()->GetActorLocation(), 
    		25, 
    		FColor::Red, 
    		false, 
    		BSConsoleVariables::CVarBSDebugHitDetectionDuration.GetValueOnGameThread());
    	}
        Result.AddUnique(Actor);
    }

    return Result;
}

void UBSAbilityFunctionLibrary::DrawDebugSlice(
	const UObject* WorldContext,
	FTransform Transform,
	TEnumAsByte<EAxis::Type> ForwardAxis,
	TEnumAsByte<EAxis::Type> UpAxis,
	float Radius,
	float Height,
	const float AngleDegrees,
	const int32 ArcSegments,
	const FColor Color,
	const float Duration,
	const float Thickness)
{
    if (!WorldContext) return;
	
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	
	DrawDebugCoordinateSystem(World, Transform.GetLocation(), Transform.GetRotation().Rotator(), Radius * 0.5, false, Duration, 0, Thickness);
	
	Radius *= Transform.GetScale3D().Y;
	Height *= Transform.GetScale3D().Z;
	
	const FVector ForwardDir = GetSwizzledDirectionFromRotation(ForwardAxis, Transform.GetRotation());
	
	FVector UpDir = GetSwizzledDirectionFromRotation(UpAxis, Transform.GetRotation());
	
    const float HalfHeight = Height * 0.5f;

    const FVector TopOrigin = Transform.GetLocation() + UpDir * HalfHeight;
    const FVector BottomOrigin = Transform.GetLocation() - UpDir * HalfHeight;

	const bool IsCircle = AngleDegrees >= 360 || AngleDegrees <= -360;
	
    const float HalfAngle = AngleDegrees * 0.5f;
	
	//Draw debug bounding box 
	/*
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
	*/
	
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

TSubclassOf<UGameplayAbility> UBSAbilityFunctionLibrary::BP_GetAbilityByWeight(UAbilitySystemComponent* Asc,
	const TMap<TSubclassOf<UGameplayAbility>, float> Map)
{
	return GetAbilityByWeight(Asc, Map);
}

TSubclassOf<UGameplayAbility> UBSAbilityFunctionLibrary::GetAbilityByWeight(UAbilitySystemComponent* Asc,
                                                                            const TMap<TSubclassOf<UGameplayAbility>, float>& Map)
{
	float TotalWeight = 0.f;
	
	TArray<TPair<TSubclassOf<UGameplayAbility>, float>> Valid;
	
	for (const auto& Pair : Map)
	{
		Asc->GiveAbility(Pair.Key);
		if (Pair.Value > 0 && CanActivateAbility(Asc, Pair.Key))
		{
			Valid.Add(Pair);
			TotalWeight += Pair.Value;
		}
	}

	if (TotalWeight <= 0)
	{
		return nullptr; // no valid weights
	}

	float RandomValue = FMath::FRandRange(0.f, TotalWeight);

	for (const auto& Pair : Valid)
	{
		RandomValue -= Pair.Value;
		if (RandomValue <= 0)
		{
			return Pair.Key;
		}
	}

	return nullptr;
}

bool UBSAbilityFunctionLibrary::CanActivateAbility(const UAbilitySystemComponent* Asc,
	const TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!Asc || !AbilityClass)
	{
		return false;
	}

	const FGameplayAbilitySpec* Spec =
		Asc->FindAbilitySpecFromClass(AbilityClass);

	if (!Spec)
	{
		return false; // Not granted
	}
	
	if (UGameplayAbility* AbilityCDO = Spec->GetPrimaryInstance())
	{
		return AbilityCDO->CommitCheck(
			AbilityCDO->GetCurrentAbilitySpecHandle(), 
			AbilityCDO->GetCurrentActorInfo(), 
			AbilityCDO->GetCurrentActivationInfo());
	}
	
	return false;
}
