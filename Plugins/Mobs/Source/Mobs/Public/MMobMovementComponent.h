// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MMobMovementComponent.generated.h"

															 
UCLASS(BlueprintType, Blueprintable, DisplayName="Mob Movement Component Base")
class MOBS_API UMMobMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	
	UMMobMovementComponent();
	
	virtual float GetMaxSpeed() const override;
	
	virtual float GetMaxSpeedForNavMovement() const override;
	
	virtual float GetSpeedModifier() const;
	
	virtual bool CanMove() const { return true; }
	
	virtual FRotator ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const override;
	
protected:
	
	UPROPERTY()
	bool bNormalWalking = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0))
	float NormalWalkSpeed = 200;
};
