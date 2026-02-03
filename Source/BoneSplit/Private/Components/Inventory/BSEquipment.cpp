// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Inventory/BSEquipment.h"

#include "GameSettings/BSDeveloperSettings.h"
#include "BoneSplit/BoneSplit.h"


UBSEquipmentEffect::UBSEquipmentEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	//Make sure this is stackable to enforce single application only
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	StackingType = EGameplayEffectStackingType::AggregateByTarget; 
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	StackLimitCount = 1;
}

TSoftObjectPtr<UTexture2D> UBSEquipmentEffect::GetIcon_Implementation() const
{
	return Icon;
}

#if WITH_EDITOR

#include "Misc/DataValidation.h"

bool UBSEquipmentEffect::CanEditChange(const FProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSEquipmentEffect, DurationPolicy))
	{
		return false;
	}
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSEquipmentEffect, StackingType))
	{
		return false;
	}
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSEquipmentEffect, StackLimitCount))
	{
		return false;
	}
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSEquipmentEffect, Period))
	{
		return false;
	}
	return ParentVal;
}

EDataValidationResult UBSEquipmentEffect::IsDataValid(FDataValidationContext& Context) const
{
	if (!IsInBlueprint()) return Super::IsDataValid(Context);
	if (!SlotTag.IsValid())
	{
		Context.AddError(FText::FromString("No valid slot tag defined"));
	}
	if (SlotTag.MatchesTagExact(BSTags::Equipment_Head))
	{
		if (!MeshInfo.ContainsByPredicate([this](const FBSEffectMeshInfo& MeshInfo)
		{
			return MeshInfo.SlotTag.MatchesTagExact(BSTags::EquipmentMesh_Head);
		}))
		{
			Context.AddWarning(FText::FromString("Head Slot expects Mesh, none found!"));
		}
	}
	if (SlotTag.MatchesTagExact(BSTags::Equipment_Chest))
	{
		if (!MeshInfo.ContainsByPredicate([this](const FBSEffectMeshInfo& MeshInfo)
		{
			return MeshInfo.SlotTag.MatchesTagExact(BSTags::EquipmentMesh_Chest);
		}))
		{
			Context.AddWarning(FText::FromString("Chest Slot expects Mesh, none found!"));
		}
	}
	if (SlotTag.MatchesTagExact(BSTags::Equipment_Legs))
	{
		if (!MeshInfo.ContainsByPredicate([this](const FBSEffectMeshInfo& MeshInfo)
		{
			return MeshInfo.SlotTag.MatchesTagExact(BSTags::EquipmentMesh_Legs);
		}))
		{
			Context.AddWarning(FText::FromString("Legs Slot expects Mesh, none found!"));
		}
	}
	if (SlotTag.MatchesTagExact(BSTags::Equipment_Arms))
	{
		if (!MeshInfo.ContainsByPredicate([this](const FBSEffectMeshInfo& MeshInfo)
		{
			return MeshInfo.SlotTag.MatchesTagExact(BSTags::EquipmentMesh_MainHand) 
			|| MeshInfo.SlotTag.MatchesTagExact(BSTags::EquipmentMesh_Offhand);
		}))
		{
			Context.AddWarning(FText::FromString("Weapon Slot expects a mesh asset in either MainHand or Offhand"));
		}
	}
	return Super::IsDataValid(Context);
}

#endif
