// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "NativeGameplayTags.h"
#include "Factions/FactionInterface.h"
#include "GameFramework/Actor.h"
#include "BSBreakable.generated.h"

class UBSExtendedAttributeSet;
class UAbilitySystemComponent;
struct FGameplayTagContainer;
class UCharacterAbilitySystem;

namespace BSGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faction_Breakable)
}

UCLASS()
class BONESPLIT_API ABSBreakable : public AActor, public IAbilitySystemInterface, public IGameplayTagAssetInterface, public IFactionInterface
{
	GENERATED_BODY()

public:
	
	explicit ABSBreakable(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	virtual FGameplayTagContainer& GetFactionTags() override { return FactionTags; }
	virtual bool HasAnyMatchingFactionTag(FGameplayTagContainer InFactionTags) override;

protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer FactionTags = FGameplayTagContainer(BSGameplayTags::Faction_Breakable);
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="On Death")
	void BP_OnDeath();
	
	virtual void OnDeath(UAbilitySystemComponent*, UAbilitySystemComponent*, FGameplayTagContainer, float, float);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, ClampMax=1, Units=Percent))
	float BreakableSpawnChance = 1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, ClampMax=1, Units=Percent))
	float SpawnChance = 0.5f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 MaxSpawns = 3;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0))
	float Health = 20;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UCharacterAbilitySystem> CharacterAbilitySystem;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSExtendedAttributeSet> AttributeSet;
};
