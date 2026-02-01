// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSTalentComponent.generated.h"


class UGameplayEffect;
struct FActiveGameplayEffectHandle;
struct FActiveGameplayEffect;
struct FBSTalentSaveData;
class UBSTalentEffect;
class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSTalentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSTalentComponent();
	
	virtual void SetAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent);
	
	//Best way to load in talents from save. Will avoid sending many RPCs.
	UFUNCTION(Server, Reliable)
	void Server_GrantSavedTalents(const TArray<FBSTalentSaveData>& TalentData);
	
	UFUNCTION(Server, Reliable)
	void Server_GrantTalents(const TArray<TSubclassOf<UBSTalentEffect>>& TalentData);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ResetTalents();
	
	/*
	 * Grants or levels up existing talent. This does NOT check for requirements, 
	 * call CanUnlockOrUpgradeTalent() where needed.
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_LevelUpTalent(TSubclassOf<UBSTalentEffect> TalentData);
	
	/*
	 * Downgrades or removes talent if level is zero.
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_LevelDownTalent(TSubclassOf<UBSTalentEffect> TalentData);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetTalentLevel(TSubclassOf<UBSTalentEffect> TalentData, int32 NewLevel);
	
	int32 GetTalentLevel(TSubclassOf<UBSTalentEffect> TalentData);
	
	bool IsMaxLevel(TSubclassOf<UBSTalentEffect> TalentData);
	
	bool IsTalentActive(TSubclassOf<UBSTalentEffect> TalentData);
	
	bool CanUnlockOrUpgradeTalent(TSubclassOf<UBSTalentEffect> TalentData);
	
	TMap<TSubclassOf<UBSTalentEffect>, int32> GetTalentData();

protected:
	
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;
	
	virtual const FActiveGameplayEffect* GetEffectHandleFromClass(TSubclassOf<UGameplayEffect> EffectClass);
	
};
