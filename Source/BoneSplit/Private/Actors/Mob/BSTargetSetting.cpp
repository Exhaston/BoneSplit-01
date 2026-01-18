// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Mob/BSTargetSetting.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/AbilitySystem/BSAbilityFunctionLibrary.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Components/Targeting/BSThreatComponent.h"
#include "Components/Targeting/BSThreatInterface.h"

bool FBSTargetFilter::RunFilter(AActor* AvatarActor, AActor* TestActor)
{
	if (!AvatarActor || !TestActor)
	{
		return false;
	}
	
	if (AvatarActor == TestActor)
	{
		return Tests.Contains(Self);
	}
	
	if (Tests.Contains(EBSTargetTestFilter::TestReachable))
	{
		if (!TestReachable(AvatarActor, TestActor))
		{
			return false; //Not reachable
		}
	}
	if (Tests.Contains(EBSTargetTestFilter::TestVisibility))
	{
		if (!TestVisibility(AvatarActor, TestActor))
		{
			return false; //Not Visible
		}
	}
	
	if (DirectionCheck.IsSet())
	{
		const FVector Forward = TestActor->GetActorForwardVector().GetSafeNormal2D();
		const FVector ToTarget =
			(AvatarActor->GetActorLocation() - TestActor->GetActorLocation()).GetSafeNormal2D();

		const float Dot = FVector::DotProduct(Forward, ToTarget);

		if (DirectionCheck.GetValue() == Ebs_Forward && Dot < 0)
		{
			return false;
		}

		if (DirectionCheck.GetValue() == Ebs_Behind && Dot > 0)
		{
			return false;
		}
	}
		
	const bool bKeepFriendlies = Tests.Contains(EBSTargetTestFilter::Friendly);
	const bool bKeepEnemies = Tests.Contains(EBSTargetTestFilter::Enemy);
	
	if (bKeepFriendlies || bKeepEnemies)
	{
		bool bHasMatchingFactions = false;
		
		if (bKeepFriendlies || bKeepEnemies)
		{
			bHasMatchingFactions = TestFaction(AvatarActor, TestActor);
		}
			
		const bool bIsFriendly = bHasMatchingFactions;
		const bool bIsEnemy = !bHasMatchingFactions;

		if (!((bKeepFriendlies && bIsFriendly) ||
			  (bKeepEnemies && bIsEnemy)))
		{
			return false;
		}                          
	}
	
	return true;
}

bool FBSTargetFilter::TestReachable(AActor* AvatarActor, const AActor* TargetActor)
{
	return UBSAbilityFunctionLibrary::CanMobReachLocation(AvatarActor, TargetActor->GetActorLocation());
}

bool FBSTargetFilter::TestVisibility(AActor* AvatarActor, const AActor* TargetActor)
{
	return UBSAbilityFunctionLibrary::CheckTargetVisibility(
	AvatarActor, AvatarActor->GetActorLocation(), TargetActor);
}

bool FBSTargetFilter::TestFaction(AActor* AvatarActor, AActor* TargetActor)
{
	const IAbilitySystemInterface* AvatarAscInterface = Cast<IAbilitySystemInterface>(AvatarActor);
	const IAbilitySystemInterface* TargetAscInterface = Cast<IAbilitySystemInterface>(TargetActor);
	
	if (!AvatarAscInterface || !TargetAscInterface) return false;
	
	return UBSAbilityFunctionLibrary::FilterByAnyMatchingFactions(
		AvatarAscInterface->GetAbilitySystemComponent(), TargetAscInterface->GetAbilitySystemComponent());
}
AActor* UBSTargetSetting_RandomInRange::GetTarget(AActor* AvatarActor)
{
	const TArray<AActor*> TargetActors = 
		UBSAbilityFunctionLibrary::GetActorsInRadius(AvatarActor, Range,true);

	TArray<AActor*> FilteredTargets;
	for (auto TargetActor : TargetActors)
	{
		if (TargetFilter.RunFilter(AvatarActor, TargetActor))
		{
			FilteredTargets.Add(TargetActor);
		}
	}
	
	if (FilteredTargets.IsEmpty()) return nullptr;
	
	return FilteredTargets[FMath::RandRange(0, FilteredTargets.Num() - 1)];
}

AActor* UBSTargetSetting_Closest::GetTarget(AActor* AvatarActor)
{
	const TArray<AActor*> TargetActors = 
		UBSAbilityFunctionLibrary::GetActorsInRadius(AvatarActor, Range,true);
	
	float CurrentRange = FLT_MAX;
	AActor* Target = nullptr;
	
	for (const auto TargetActor : TargetActors)
	{
		if (TargetFilter.RunFilter(AvatarActor, TargetActor))
		{
			const float RangeSquared = 
				FVector::DistSquared(AvatarActor->GetActorLocation(), TargetActor->GetActorLocation());
			if (CurrentRange > RangeSquared)
			{
				CurrentRange = RangeSquared;
				Target = TargetActor;
			}
		}
	}
	
	return Target;
}

AActor* UBSTargetSetting_Farthest::GetTarget(AActor* AvatarActor)
{
	const TArray<AActor*> TargetActors = 
		UBSAbilityFunctionLibrary::GetActorsInRadius(AvatarActor, Range,true);
	
	float CurrentRange = FLT_MIN;
	AActor* Target = nullptr;
	
	for (const auto TargetActor : TargetActors)
	{
		if (TargetFilter.RunFilter(AvatarActor, TargetActor))
		{
			const float RangeSquared = 
				FVector::DistSquared(AvatarActor->GetActorLocation(), TargetActor->GetActorLocation());
			if (CurrentRange < RangeSquared)
			{
				CurrentRange = RangeSquared;
				Target = TargetActor;
			}
		}
	}
	
	return Target;
}

AActor* UBSTargetSetting_RandomThreat::GetTarget(AActor* AvatarActor)
{
	IBSThreatInterface* ThreatInterface = Cast<IBSThreatInterface>(AvatarActor);
	if (!ThreatInterface) return nullptr;
	
	TMap<TWeakObjectPtr<AActor>, float>& ThreatMap = ThreatInterface->GetThreatComponent()->GetThreatMap();
	
	TArray<AActor*> FilteredActors;
	
	for (const auto ThreatPair : ThreatMap)
	{
		if (!ThreatPair.Key.IsValid()) continue;
		if (TargetFilter.RunFilter(AvatarActor, ThreatPair.Key.Get()))
		{
			FilteredActors.Add(ThreatPair.Key.Get());
		}
	}
	
	if (FilteredActors.IsEmpty()) return nullptr;
	
	return UBSAbilityFunctionLibrary::PickRandomTargetFromArray(FilteredActors);
}

AActor* UBSTargetSetting_LowestThreat::GetTarget(AActor* AvatarActor)
{
	IBSThreatInterface* ThreatInterface = Cast<IBSThreatInterface>(AvatarActor);
	if (!ThreatInterface) return nullptr;
	
	TMap<TWeakObjectPtr<AActor>, float>& ThreatMap = ThreatInterface->GetThreatComponent()->GetThreatMap();
	
	float CurrentThreat = FLT_MAX;
	AActor* Target = nullptr;
	
	for (const auto ThreatPair : ThreatMap)
	{
		if (!ThreatPair.Key.IsValid()) continue;
		if (ThreatPair.Value < CurrentThreat)
		{
			if (TargetFilter.RunFilter(AvatarActor,ThreatPair.Key.Get()))
			{
				CurrentThreat = ThreatPair.Value;
				Target = ThreatPair.Key.Get();
			}
		}
	}
	
	return Target;
}

AActor* UBSTargetSetting_HighestThreat::GetTarget(AActor* AvatarActor)
{
	IBSThreatInterface* ThreatInterface = Cast<IBSThreatInterface>(AvatarActor);
	if (!ThreatInterface) return nullptr;
	
	TMap<TWeakObjectPtr<AActor>, float>& ThreatMap = ThreatInterface->GetThreatComponent()->GetThreatMap();
	
	float CurrentThreat = FLT_MIN;
	AActor* Target = nullptr;
	
	for (const auto ThreatPair : ThreatMap)
	{
		if (!ThreatPair.Key.IsValid()) continue;
		if (ThreatPair.Value > CurrentThreat)
		{
			if (TargetFilter.RunFilter(AvatarActor,ThreatPair.Key.Get()))
			{
				CurrentThreat = ThreatPair.Value;
				Target = ThreatPair.Key.Get();
			}
		}
	}
	
	return Target;
}

AActor* UBSTargetSetting_ClosestThreat::GetTarget(AActor* AvatarActor)
{
	IBSThreatInterface* ThreatInterface = Cast<IBSThreatInterface>(AvatarActor);
	if (!ThreatInterface) return nullptr;
	
	TMap<TWeakObjectPtr<AActor>, float>& ThreatMap = ThreatInterface->GetThreatComponent()->GetThreatMap();
	
	float CurrentRange = FLT_MAX;
	AActor* Target = nullptr;
	
	for (const auto ThreatPair : ThreatMap)
	{
		if (!ThreatPair.Key.IsValid()) continue;
		
		const float RangeSquared = 
			FVector::DistSquared(ThreatPair.Key->GetActorLocation(), AvatarActor->GetActorLocation());

		if (RangeSquared < CurrentRange)
		{
			if (TargetFilter.RunFilter(AvatarActor,ThreatPair.Key.Get()))
			{
				CurrentRange = RangeSquared;
				Target = ThreatPair.Key.Get();
			}
		}
	}
	
	return Target;
}

AActor* UBSTargetSetting_FarthestThreat::GetTarget(AActor* AvatarActor)
{
	IBSThreatInterface* ThreatInterface = Cast<IBSThreatInterface>(AvatarActor);
	if (!ThreatInterface) return nullptr;
	
	TMap<TWeakObjectPtr<AActor>, float>& ThreatMap = ThreatInterface->GetThreatComponent()->GetThreatMap();
	
	float CurrentRange = FLT_MIN;
	AActor* Target = nullptr;
	
	for (const auto ThreatPair : ThreatMap)
	{
		if (!ThreatPair.Key.IsValid()) continue;
		const float RangeSquared = 
			FVector::DistSquared(ThreatPair.Key->GetActorLocation(), AvatarActor->GetActorLocation());

		if (RangeSquared > CurrentRange)
		{
			if (TargetFilter.RunFilter(AvatarActor,ThreatPair.Key.Get()))
			{
				CurrentRange = RangeSquared;
				Target = ThreatPair.Key.Get();
			}
		}
	}
	
	return Target;
}

AActor* UBSTargetSetting_LowestHealthThreat::GetTarget(AActor* AvatarActor)
{
	IBSThreatInterface* ThreatInterface = Cast<IBSThreatInterface>(AvatarActor);
	if (!ThreatInterface) return nullptr;
	
	TMap<TWeakObjectPtr<AActor>, float>& ThreatMap = ThreatInterface->GetThreatComponent()->GetThreatMap();
	
	float CurrentHealthPercent = FLT_MAX;
	AActor* Target = nullptr;

	for (const auto PossibleTargetPair : ThreatMap)
	{
		if (!PossibleTargetPair.Key.IsValid()) continue;
		if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(PossibleTargetPair.Key.Get()))
		{
			const UAbilitySystemComponent* Asc = AscInterface->GetAbilitySystemComponent();
			check(Asc);
			const float FoundHealth = Asc->GetNumericAttribute(UBSAttributeSet::GetHealthAttribute());
			const float MaxHealth = Asc->GetNumericAttribute(UBSAttributeSet::GetMaxHealthAttribute());

			const float HealthPercent = (MaxHealth > 0) ? (FoundHealth / MaxHealth) * 100 : 0;
			
			if (HealthPercent > MaxHealthPercent) continue;
			
			if (HealthPercent < CurrentHealthPercent)
			{
				if (TargetFilter.RunFilter(AvatarActor,PossibleTargetPair.Key.Get()))
				{
					CurrentHealthPercent = HealthPercent;
					Target = PossibleTargetPair.Key.Get();
				}
			}
		}
	}
	
	return Target;
}

AActor* UBSTargetSetting_HighestHealthThreat::GetTarget(AActor* AvatarActor)
{
	IBSThreatInterface* ThreatInterface = Cast<IBSThreatInterface>(AvatarActor);
	if (!ThreatInterface) return nullptr;
	
	TMap<TWeakObjectPtr<AActor>, float>& ThreatMap = ThreatInterface->GetThreatComponent()->GetThreatMap();
	
	float CurrentHealthPercent = FLT_MIN;
	AActor* Target = nullptr;

	for (const auto PossibleTargetPair : ThreatMap)
	{
		if (!PossibleTargetPair.Key.IsValid()) continue;
		if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(PossibleTargetPair.Key))
		{
			const UAbilitySystemComponent* Asc = AscInterface->GetAbilitySystemComponent();
			check(Asc);
			const float FoundHealth = Asc->GetNumericAttribute(UBSAttributeSet::GetHealthAttribute());
			const float MaxHealth = Asc->GetNumericAttribute(UBSAttributeSet::GetMaxHealthAttribute());

			const float HealthPercent = (MaxHealth > 0) ? (FoundHealth / MaxHealth) * 100 : 0;
			
			if (HealthPercent < MinHealthPercent) continue;
			
			if (HealthPercent > CurrentHealthPercent)
			{
				if (TargetFilter.RunFilter(AvatarActor,PossibleTargetPair.Key.Get()))
				{
					CurrentHealthPercent = HealthPercent;
					Target = PossibleTargetPair.Key.Get();
				}
			}
		}
	}
	
	return Target;
}

AActor* UBSTargetSetting_LowestHealthRadius::GetTarget(AActor* AvatarActor)
{
	const TArray<AActor*> TargetActors = 
		UBSAbilityFunctionLibrary::GetActorsInRadius(AvatarActor, Range,true);
	
	float CurrentHealthPercent = FLT_MAX;
	AActor* Target = nullptr;

	for (const auto PossibleTarget : TargetActors)
	{
		if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(PossibleTarget))
		{
			const UAbilitySystemComponent* Asc = AscInterface->GetAbilitySystemComponent();
			check(Asc);
			const float FoundHealth = Asc->GetNumericAttribute(UBSAttributeSet::GetHealthAttribute());
			const float MaxHealth = Asc->GetNumericAttribute(UBSAttributeSet::GetMaxHealthAttribute());

			const float HealthPercent = (MaxHealth > 0) ? (FoundHealth / MaxHealth) * 100 : 0;
			
			if (HealthPercent > MaxHealthPercent) continue;
			
			if (HealthPercent < CurrentHealthPercent)
			{
				if (TargetFilter.RunFilter(AvatarActor,PossibleTarget))
				{
					CurrentHealthPercent = HealthPercent;
					Target = PossibleTarget;
				}
			}
		}
	}
	
	return Target;
}

AActor* UBSTargetSetting_HighestHealthRadius::GetTarget(AActor* AvatarActor)
{
	const TArray<AActor*> TargetActors = UBSAbilityFunctionLibrary::GetActorsInRadius(AvatarActor, Range, true);
	
	float CurrentHealthPercent = FLT_MIN;
	AActor* Target = nullptr;

	for (const auto PossibleTarget : TargetActors)
	{
		if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(PossibleTarget))
		{
			const UAbilitySystemComponent* Asc = AscInterface->GetAbilitySystemComponent();
			check(Asc);
			const float FoundHealth = Asc->GetNumericAttribute(UBSAttributeSet::GetHealthAttribute());
			const float MaxHealth = Asc->GetNumericAttribute(UBSAttributeSet::GetMaxHealthAttribute());

			const float HealthPercent = (MaxHealth > 0) ? (FoundHealth / MaxHealth) * 100 : 0;
			
			if (HealthPercent < MinHealthPercent) continue;
			
			if (HealthPercent > CurrentHealthPercent)
			{
				if (TargetFilter.RunFilter(AvatarActor,PossibleTarget))
				{
					CurrentHealthPercent = HealthPercent;
					Target = PossibleTarget;
				}
			}
		}
	}
	
	return Target;
}
