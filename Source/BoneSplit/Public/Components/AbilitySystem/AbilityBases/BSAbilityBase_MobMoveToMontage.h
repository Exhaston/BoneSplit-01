// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSAbilityBase_MobAbility.h"
#include "BSAbilityBase_MobMoveToMontage.generated.h"

class AAIController;

namespace EPathFollowingResult
{
	enum Type : int;
}

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSAbilityBase_MobMoveToMontage : public UBSAbilityBase_MobAbility
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	void StartMoveToAbility();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* Montage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MontageRate = 1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MontageRootMotionScale = 1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float AcceptanceRadius = 200;
	

	

};
