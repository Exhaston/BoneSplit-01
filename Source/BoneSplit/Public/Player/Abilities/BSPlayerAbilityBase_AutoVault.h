// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPlayerAbilityBase.h"
#include "BSPlayerAbilityBase_AutoVault.generated.h"

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSPlayerAbilityBase_AutoVault : public UBSPlayerAbilityBase
{
	GENERATED_BODY()
	
public:
	
	UBSPlayerAbilityBase_AutoVault();
	
	bool bReady = false;
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION()
	void OnCharacterMovementModeChanged(ACharacter* Character, EMovementMode NewMovementMode, uint8 CustomIndex);
	
	UFUNCTION()
	void OnAbilityTick(float DeltaSeconds);
	
	bool TryDetectVault(FVector& OutLedgeTop, FVector& OutLedgeNormal) const;
};
