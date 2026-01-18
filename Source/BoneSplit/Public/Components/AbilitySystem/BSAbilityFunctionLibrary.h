// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BSAbilityFunctionLibrary.generated.h"

class UBSGameplayEffect;
struct FBSTargetFilter;
class UBSTargetSetting;
class UGameplayAbility;
class UGameplayEffect;
class UBSThreatComponent;
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
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static bool FilterByAnyMatchingFactions(UAbilitySystemComponent* Asc, UAbilitySystemComponent* OtherAsc);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static bool FilterByNoMatchingFactions(UAbilitySystemComponent* Asc, UAbilitySystemComponent* OtherAsc);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static void TryActivateAbilityWeighted(UAbilitySystemComponent* OwnerAsc, TMap<TSubclassOf<UGameplayAbility>, float> AbilityMap);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat", meta=(DefaultToSelf="Ability", AdvancedDisplay="EffectLevel, bTestVisibility"))
	static void LazyApplyEffectToTarget(
		UGameplayAbility* Ability, 
		AActor* TargetActor, 
		TSubclassOf<UBSGameplayEffect> EffectClass, 
		FVector Origin,
		FBSTargetFilter Filter,
		float EffectLevel = 1);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static bool CanMobReachLocation(AActor* InMob, FVector TargetLocation);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static bool CheckMobGrounded(AActor* InMob);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static TArray<AActor*> FilterArrayByReachable(AActor* InOriginActor, TArray<AActor*> InActors);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static TArray<AActor*> FilterArrayByVisible(UObject* WorldContext, FVector InOrigin, TArray<AActor*> InActors);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static bool CheckTargetVisibility(UObject* WorldContext, FVector InOrigin, const AActor* InTargetActor);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static TArray<AActor*> GetActorsInRadius(
		AActor* InOriginActor, float InRange, bool bIncludeSelf = false);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static AActor* PickRandomTargetFromArray(TArray<AActor*> InActors);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static AActor* PickClosestTargetFromArray(AActor* InOriginActor, TArray<AActor*> InActors);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static AActor* PickFarthestTargetFromArray(AActor* InOriginActor, TArray<AActor*> InActors);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static TArray<AActor*> GetThreatActors(AActor* InTarget);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static AActor* GetHighestThreatActor(AActor* InTarget);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static AActor* GetLowestThreatActor(AActor* InTarget);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat")
	static UBSThreatComponent* GetThreatComponent(AActor* InTarget);
	
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

	
	UFUNCTION(BlueprintCallable, Category="BoneSplit")
	static TSubclassOf<UGameplayAbility> BP_GetAbilityByWeight(
		UAbilitySystemComponent* Asc, 
		const TMap<TSubclassOf<UGameplayAbility>, float> Map);
	static TSubclassOf<UGameplayAbility> GetAbilityByWeight(
	UAbilitySystemComponent* Asc, const TMap<TSubclassOf<UGameplayAbility>, float>& Map);

	static bool CanActivateAbility(const UAbilitySystemComponent* Asc, TSubclassOf<UGameplayAbility> AbilityClass);
};
