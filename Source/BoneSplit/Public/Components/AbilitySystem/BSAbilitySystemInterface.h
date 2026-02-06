// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "BSAbilityLibrary.h"
#include "UObject/Interface.h"
#include "BSAbilitySystemInterface.generated.h"

class UBSThreatComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBSOnDamageOther, FGameplayAttribute, Attribute, float, Damage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FBSOnDeathDelegate, UAbilitySystemComponent*, SourceAsc, UAbilitySystemComponent*, TargetAsc, float, Damage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FBSOnCombatChangedDelegate, bool, bIsInCombat);

UINTERFACE(Blueprintable, BlueprintType)
class UBSAbilitySystemInterface : public UAbilitySystemInterface
{
	GENERATED_BODY()
};

class BONESPLIT_API IBSAbilitySystemInterface : public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	//Should be called for all clients and server
	virtual FBSOnDeathDelegate& GetOnDeathDelegate() = 0;
	
	//Called from the server when the entity has no health left. Run RPCs or states from this.
	virtual void Die(UAbilitySystemComponent* SourceAsc, float Damage) = 0;
	
	//Called for clients and server
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnDeath", Category="Ability Interface")
	void BP_OnDeath(UAbilitySystemComponent* SourceAsc, float Damage);
	
	virtual void BP_OnDeath_Implementation(UAbilitySystemComponent* SourceAsc, float Damage) = 0;
	                                                        
	//Called from server to launch the character
	virtual void Launch(FVector LaunchVelocity, bool bAdditive) = 0;
	
	//Called for clients and server
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnLaunched", Category="Ability Interface")
	void BP_OnLaunched(FVector LaunchVelocity);
	
	virtual void BP_OnLaunched_Implementation(FVector LaunchVelocity) = 0;
	
	virtual bool IsInCombat() = 0;
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="IsInCombat", Category="Ability Interface")
	bool BP_IsInCombat();
	
	virtual bool BP_IsInCombat_Implementation() = 0;
	
	virtual FBSOnCombatChangedDelegate& GetOnCombatChangedDelegate() = 0;
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnCombatChanged", Category="Ability Interface")
	void BP_OnCombatChanged(bool InCombat);
	
	virtual void BP_OnCombatChanged_Implementation(bool InCombat) = 0;
};
