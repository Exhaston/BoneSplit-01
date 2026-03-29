// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MMobMovementComponent.h"
#include "BSMobMovementComponent.generated.h"


class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSMobMovementComponent : public UMMobMovementComponent
{
	GENERATED_BODY()
	
public:
	
	UBSMobMovementComponent();
	
	virtual void InitializeComponent() override;
	
	virtual float GetSpeedModifier() const override;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> OwnerAbilitySystem;
};
