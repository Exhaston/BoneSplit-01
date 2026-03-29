// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/Equipment/BSEquipmentInstance.h"

#include "AbilitySystemComponent.h"
#include "Player/Equipment/BSEquipmentAsset.h"
#include "Player/Equipment/BSEquipmentSkeletalMeshComponent.h"

FBSEquipmentInstance::FBSEquipmentInstance(const UBSEquipmentAsset* EquipmentAsset)
{
	EquipmentDef = EquipmentAsset;
}

void FBSEquipmentInstance::SetLevel(UAbilitySystemComponent* InAsc, const int32 InLevel)
{
	check(InAsc);
	
	EquipmentLevel = InLevel;
	
	if (!InAsc->IsOwnerActorAuthoritative()) return;
	
	for (const auto EffectSpecHandle : GrantedEffectHandles)
	{
		if (const FActiveGameplayEffect* ActiveHandle = InAsc->GetActiveGameplayEffect(EffectSpecHandle))
		{
			InAsc->SetActiveGameplayEffectLevel(ActiveHandle->Handle, InLevel);
		}
	}
	for (const auto AbilitySpecHandle : GrantedSpecHandles)
	{
		InAsc->FindAbilitySpecFromHandle(AbilitySpecHandle)->Level = InLevel;
	}
}

void FBSEquipmentInstance::ApplyEquipmentInstance(UAbilitySystemComponent* InAsc,
	USkeletalMeshComponent* ParentMeshComp)
{
	check(InAsc);
	check(ParentMeshComp);
	
	if (InAsc->IsOwnerActorAuthoritative())
	{
		ApplyEffects(InAsc);
		GrantAbilities(InAsc);
	}
	
	CreateMeshInstances(ParentMeshComp);
}

void FBSEquipmentInstance::ApplyEffects(UAbilitySystemComponent* InAsc)
{
	check(InAsc);
	for (const auto AppliedEffect : GetDef()->AppliedEffects)
	{
		InAsc->BP_ApplyGameplayEffectToSelf(AppliedEffect, GetEquipmentLevel(), InAsc->MakeEffectContext());
	}
}

void FBSEquipmentInstance::GrantAbilities(UAbilitySystemComponent* InAsc)
{
	check(InAsc);
	for (const auto GrantedAbility : GetDef()->GrantedAbilities)
	{
		InAsc->GiveAbility(GrantedAbility);
	}
}

void FBSEquipmentInstance::CreateMeshInstances(USkeletalMeshComponent* ParentMesh)
{

}

void FBSEquipmentInstance::ClearEquipmentInstance(UAbilitySystemComponent* InAsc)
{
	check(InAsc);
	
	if (InAsc->IsOwnerActorAuthoritative())
	{
		for (auto GrantedSpecHandle : GrantedSpecHandles)
		{
			InAsc->ClearAbility(GrantedSpecHandle);
		}

		for (const auto GrantedEffectHandle : GrantedEffectHandles)
		{
			InAsc->RemoveActiveGameplayEffect(GrantedEffectHandle, 1);
		}
	}
	
	for (const auto MeshCompInstance : MeshComponentInstances)
	{
		MeshCompInstance->DestroyComponent();
	}
}

int32 FBSEquipmentInstance::GetEquipmentLevel() const
{
	return EquipmentLevel;
}

const UBSEquipmentAsset* FBSEquipmentInstance::GetDef() const
{
	return EquipmentDef;
}

void FBSEquipmentInstance::SetMeshColor(const FColor InColor)
{
	for (const auto& MeshComponent : MeshComponentInstances)
	{
		MeshComponent->SetMaterialColor(InColor);
	}
}

bool FBSEquipmentInstance::HasValidData() const
{
	return IsValid(GetDef()) && GetEquipmentLevel() >= 0;
}
