// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSFiniteState.h"
#include "BSMobState.generated.h"

class UBSThreatComponent;
class AAIController;
class ABSMobCharacter;

/**
 * Finite State designed to work with mob characters only. It has accessors for frequently used AI components.
 */
UCLASS(DisplayName="Mob Finite State", Category="BoneSplit")
class BONESPLIT_API UBSMobState : public UBSFiniteState
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeEnterState(UBSFiniteStateComponent* InOwnerComponent) override;
	virtual void NativeEndState(UBSFiniteStateComponent* InOwnerComponent) override;
	
	UFUNCTION(BlueprintPure, Category="BoneSplit|MobFiniteState")
	ABSMobCharacter* GetMobCharacter() const;
	
	UFUNCTION(BlueprintPure, Category="BoneSplit|MobFiniteState")
	AAIController* GetMobController() const;
	
	UFUNCTION(BlueprintPure, Category="BoneSplit|MobFiniteState")
	UBSThreatComponent* GetMobThreatComponent() const;
	
	UFUNCTION(BlueprintPure, Category="BoneSplit|MobFiniteState")
	UAbilitySystemComponent* GetMobAbilitySystemComponent() const;
	
private:
	
	UPROPERTY()
	TWeakObjectPtr<AAIController> MobController;
	
	UPROPERTY()
	TWeakObjectPtr<ABSMobCharacter> MobCharacter;
};
