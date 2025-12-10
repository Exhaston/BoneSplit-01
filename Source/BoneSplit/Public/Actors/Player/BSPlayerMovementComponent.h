// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClientAuthoritativeCharacterMovementComponent.h"
#include "BSPlayerMovementComponent.generated.h"


class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSPlayerMovementComponent : public UClientAuthoritativeCharacterMovementComponent
{
	GENERATED_BODY()

public:
	
	UBSPlayerMovementComponent();
	
	virtual void BeginPlay() override;
	
	virtual float GetMaxSpeed() const override;
	
	//Multiplier to the movement speed.
	UPROPERTY()
	float CachedSpeedMod = 1;
	
	UPROPERTY()
	bool bDead = false;
	
	UPROPERTY()
	bool bMovementImpaired = false;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> OwnerAsc;
};
