// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ClientAuthoritativeCharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BSMobMovementComponent.generated.h"


class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSMobMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	
	UBSMobMovementComponent();
	
	virtual void BeginPlay() override;
	
	UPROPERTY()
	FVector LastValidLocation;
	
	virtual float GetMaxSpeed() const override;
	
	virtual bool CanMove() const;
	
	virtual void BindTags(UAbilitySystemComponent* InAsc);
	
	virtual void BindAttributes(UAbilitySystemComponent* InAsc);
	
	UPROPERTY()
	TArray<FGameplayTag> ImpairmentTags;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayAttribute SpeedAttribute = UBSAttributeSet::GetSpeedAttribute();
	
	//Multiplier to the movement speed.
	UPROPERTY()
	float CachedSpeedMod = 1;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> OwnerAsc;
};
