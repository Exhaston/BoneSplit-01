// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Components/AbilitySystem/BSAbilityLibrary.h"
#include "BSTargetingComponent.generated.h"

class UBSAbilityLibrary;
class UAbilitySystemComponent;

USTRUCT(BlueprintType, Blueprintable)
struct FBSTargetInfo
{
	GENERATED_BODY()
	
	FBSTargetInfo() = default;
	FBSTargetInfo(UAbilitySystemComponent* Owner, UAbilitySystemComponent* Target)
	{
		if (!ensure(Owner && Target && Owner->GetAvatarActor() && Target->GetAvatarActor()))
		{
			return;
		}
		
		if (Target->GetAvatarActor())
		{
			TargetAbilitySystemComponent = Target;
			Distance = FVector::Dist(
				Owner->GetAvatarActor()->GetActorLocation(), 
				Target->GetAvatarActor()->GetActorLocation());
			
			MatchedFaction = UBSAbilityLibrary::HasMatchingFaction(Owner, Target);
		}
		
		//Info.HealthPercent = MaxHealth > 0.f ? CurrentHealth / MaxHealth : 1.f;
	}
	
	bool IsTargetValid() const { return IsValid(TargetAbilitySystemComponent); }
	
	UPROPERTY()
	UAbilitySystemComponent* TargetAbilitySystemComponent = nullptr;
	
	UPROPERTY()
	bool MatchedFaction = false;
	
	UPROPERTY()
	float Score = 0;
	
	UPROPERTY()
	float Distance = 0;
	
	UPROPERTY()
	float HealthPercent  = 1;
	
	UPROPERTY()
	bool bHasLineOfSight = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UBSTargetingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
