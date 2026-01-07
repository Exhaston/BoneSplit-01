// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BSAbilityFunctionLibrary.generated.h"

class UBSAbilitySystemComponent;
class UCharacterMovementComponent;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSAbilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, DisplayName="Slice Shape Overlap", Category="BoneSplit|Abilities")
	static TArray<AActor*> SliceShapeOverlap(
		UObject* WorldContext,
		FTransform Transform,
		TEnumAsByte<EAxis::Type> ForwardAxis,
		TEnumAsByte<EAxis::Type> UpAxis,
		float Angle, 
		float Height, 
		float Distance,
		TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels);
	
	static void DrawDebugSlice(
		const UObject* WorldContext, 
		FTransform Transform,
		TEnumAsByte<EAxis::Type> ForwardAxis,
		TEnumAsByte<EAxis::Type> UpAxis,
		float Radius,
		float Height,
		float AngleDegrees,
		int32 ArcSegments = 12,
		FColor Color = FColor::Red, 
		float Duration = 1, 
		float Thickness = 1.5);
	
	//Gets the appropriate vector to the axis desired. If InAxis = None will return forward.
	static FVector GetSwizzledDirectionFromRotation(TEnumAsByte<EAxis::Type> InAxis, const FQuat& InRotation);
	
	UFUNCTION(BlueprintCallable, Category="Bone Split")
	static UBSAbilitySystemComponent* GetBSAbilitySystemComponent(AActor* ActorToTest);
};
