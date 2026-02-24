// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BSAbilityLibrary.generated.h"

struct FGameplayTagContainer;
struct FGameplayEventData;
class UBSGameplayEffect;
struct FBSTargetFilter;
class UBSTargetSetting;
class UGameplayAbility;
class UGameplayEffect;
class UBSThreatComponent;
class UBSAbilitySystemComponent;
class UCharacterMovementComponent;
class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FBSOnActorDied, AActor* Target, AActor* Source, float Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBSOnActorDiedDynamic, AActor*, Target, AActor*, Source, float, Damage);

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSAbilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="BoneSplit")
	static FVector GetRandomPointInHalfRadius2D(const FVector& Origin, const FVector& ForwardDirection, float Radius);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Data")
	static FString ConvertTagToString(FGameplayTag InTag);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Abilities")
	static UAbilitySystemComponent* GetAbilitySystemComponent(AActor* Target);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Abilities")
	static bool HasMatchingFaction(UAbilitySystemComponent* Asc, UAbilitySystemComponent* OtherAsc);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Abilities")
	static bool HasNoMatchingFaction(UAbilitySystemComponent* Asc, UAbilitySystemComponent* OtherAsc);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Abilities")
	static bool IsCharacterGrounded(ACharacter* Character);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Abilities", DisplayName="CanActorSeeTarget")
	static bool CanSeeTargetActor(AActor* OriginActor, AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Abilities", DisplayName="CheckVisibilityFromOrigin")
	static bool CanSeeTargetLocation(UWorld* World, FVector OriginLocation, FVector TargetLocation);
	                                                                                            
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Abilities", DisplayName="GetActorsInRadiusFromActor")
	static TArray<AActor*> GetActorsInRadius(AActor* InOriginActor, float Radius = 250);
	                                   
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Abilities", DisplayName="GetActorsInRadiusFromPoint")
	static TArray<AActor*> GetActorsInRadiusFromPoint(UWorld* World, FVector OriginLocation, float Radius = 250);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Abilities")
	static void TryActivateAbilityWeighted(UAbilitySystemComponent* OwnerAsc, TMap<TSubclassOf<UGameplayAbility>, float> AbilityMap);
	
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat", meta=(DefaultToSelf="Ability", AdvancedDisplay="EffectLevel, bTestVisibility"))
	static void LazyApplyEffectToTarget(
		UGameplayAbility* Ability, 
		AActor* TargetActor, 
		TSubclassOf<UBSGameplayEffect> EffectClass, 
		FVector Origin,
		FBSTargetFilter Filter,
		float EffectLevel = 1);
	
	//Define effects to apply to Event Payload. Only applies if any of the payload tags matches the Event tags.
	UFUNCTION(BlueprintCallable, Category="BoneSplit|Combat", meta=(DefaultToSelf="Ability", HidePin="Ability", AdvancedDisplay="EffectLevel"))
	static void ApplyPayloadEffect(
		UGameplayAbility* Ability,
		FGameplayTagContainer EventTags,
		FGameplayEventData Payload, 
		TArray<TSubclassOf<UBSGameplayEffect>> Effects,
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
	static TArray<AActor*> GetActorsInRadiusDep(
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
	
	template<typename KeyType>
bool GetWeightedRandomKey(const TMap<KeyType, float>& Map,KeyType& OutKey)
	{
		float TotalWeight = 0.f;

		for (const auto& Pair : Map)
		{
			if (Pair.Value > 0.f)
			{
				TotalWeight += Pair.Value;
			}
		}

		if (TotalWeight <= 0.f)
		{
			return false; // no valid weights
		}

		float RandomValue = FMath::FRandRange(0.f, TotalWeight);

		for (const auto& Pair : Map)
		{
			if (Pair.Value <= 0.f)
			{
				continue;
			}

			RandomValue -= Pair.Value;

			if (RandomValue <= 0.f)
			{
				OutKey = Pair.Key;
				return true;
			}
		}

		return false;
	}
};
