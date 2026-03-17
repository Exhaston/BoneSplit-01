// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "BSEquipmentInstance.generated.h"

class UBSEquipmentSkeletalMeshComponent;
struct FActiveGameplayEffectHandle;
class UAbilitySystemComponent;
struct FGameplayAbilitySpecHandle;
struct FGameplayEffectSpecHandle;
class UBSEquipmentAsset;

USTRUCT(BlueprintType)
struct FBSEquipmentInstance
{
	GENERATED_BODY()
	
	FBSEquipmentInstance() = default;

	explicit FBSEquipmentInstance(const UBSEquipmentAsset* EquipmentAsset);
	
	void SetLevel(UAbilitySystemComponent* InAsc, int32 InLevel);
	
	void ApplyEquipmentInstance(UAbilitySystemComponent* InAsc, USkeletalMeshComponent* ParentMeshComp);
	void ClearEquipmentInstance(UAbilitySystemComponent* InAsc);
	
	void ApplyEffects(UAbilitySystemComponent* InAsc);
	void GrantAbilities(UAbilitySystemComponent* InAsc);
	void CreateMeshInstances(USkeletalMeshComponent* ParentMesh);
	
	int32 GetEquipmentLevel() const;
	
	const UBSEquipmentAsset* GetDef() const;
	
	void SetMeshColor(FColor InColor);
	
	bool HasValidData() const;
	
	bool operator==(const FBSEquipmentInstance& Other) const
	{
		return EquipmentLevel == Other.EquipmentLevel && EquipmentDef == Other.EquipmentDef;
	}
	
	friend uint32 GetTypeHash(const FBSEquipmentInstance& Instance)
	{
		return HashCombine(GetTypeHash(Instance.EquipmentLevel), GetTypeHash(Instance.EquipmentDef));
	}
	
protected:
	
	UPROPERTY(Transient)
	TArray<FActiveGameplayEffectHandle> GrantedEffectHandles;
	
	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> GrantedSpecHandles;
	
	UPROPERTY(Transient)
	TArray<UBSEquipmentSkeletalMeshComponent*> MeshComponentInstances;
	
	UPROPERTY(meta=(ClampMin=0))
	int32 EquipmentLevel = 0;
	
	UPROPERTY(Transient)
	const UBSEquipmentAsset* EquipmentDef = nullptr;
};


