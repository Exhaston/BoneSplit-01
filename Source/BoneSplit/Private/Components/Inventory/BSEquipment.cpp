// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Inventory/BSEquipment.h"

#include "GameSettings/BSDeveloperSettings.h"

#if WITH_EDITOR

#include "BoneSplit/BoneSplit.h"
#include "Misc/DataValidation.h"
#include "UObject/ObjectSaveContext.h"

EDataValidationResult UBSEquipment::IsDataValid(FDataValidationContext& Context) const
{
	const FGameplayTagContainer ParentTags = FGameplayTagContainer::CreateFromArray(
		TArray<FGameplayTag>{BSTags::Equipment, BSTags::Equipment_Misc, BSTags::Equipment_Part});
	
	if (!SlotTag.IsValid() || SlotTag.MatchesAnyExact(ParentTags))
	{
		Context.AddError(
			FText::FromString("Slot Tag is empty or a parent tag. Please specify a valid specified Slot Tag."));
	}
	if (SlotTag.MatchesTag(BSTags::Equipment_Part) && SkeletalMesh.IsNull())
	{
		Context.AddError(FText::FromString("Slot Tag requires valid skeletal mesh!"));
	}
	if (EquipmentName.IsEmpty())
	{
		Context.AddWarning(FText::FromString("Missing Name for Equipment. Will display as empty."));
	}
	return Super::IsDataValid(Context);
}

bool UBSEquipment::CanEditChange(const FProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSEquipment, SkeletalMesh))
	{
		return SlotTag.MatchesTag(BSTags::Equipment_Part) && ParentVal;
	}
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSEquipment, OffHandSkeletalMesh))
	{
		return SlotTag.MatchesTagExact(BSTags::Equipment_Part_Weapon_Main) && ParentVal;
	}
	return ParentVal;
}

void UBSEquipment::PreSave(const FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);
	if (!SlotTag.MatchesTag(BSTags::Equipment_Part) && !SkeletalMesh.IsNull())
	{
		UE_LOG(BoneSplit, Display, 
		TEXT("%s: Pre Save Cleanup: Removed Skeletal Mesh Ptr as slot tag does not require one."), 
		*GetClass()->GetName());
		
		SkeletalMesh.Reset();
	}
}

#endif

TSoftObjectPtr<UTexture2D> UBSEquipment::GetIcon_Implementation() const
{
	return Icon;
}

bool UBSEquipment::HasSkeletalMesh() const
{
	if (!SlotTag.MatchesTag(BSTags::Equipment_Part)) return false; //not a slot tag that applies a skeletal mesh
		
	if (SkeletalMesh.IsNull()) //if this is the correct slot for a mesh, but no valid found
	{
		UE_LOG(BoneSplit, Error, 
			TEXT("%s: Expected a SkeletalMesh, but none was found or it is null!"), *GetClass()->GetName());
			
		return false;
	}
		
	return true;
}

FBSEquipmentInstance::FBSEquipmentInstance(const TSubclassOf<UBSEquipment> ParentClass, const int32 InItemLevel)
{
	SourceItemClass = ParentClass;
	bool ItemLevelSuccess;
	SetItemLevel(InItemLevel, ItemLevelSuccess);
}

FBSEquipmentInstance::FBSEquipmentInstance(const TSubclassOf<UBSEquipment> ParentClass)
{
	SourceItemClass = ParentClass;
	bool ItemLevelSuccess;
	SetItemLevel(1, ItemLevelSuccess);
}

FText FBSEquipmentInstance::GetItemName() const
{
	return GetSourceItemCDO()->EquipmentName;
}

FText FBSEquipmentInstance::GetItemDescription() const
{
	//We could just grab the leaf tag name, but it's important to be able to localize these too.
	const UBSDeveloperSettings* DevSettingsCDO = GetDefault<UBSDeveloperSettings>();
	FText TagName = FText::FromName(GetSourceItemCDO()->SlotTag.GetTagLeafName());
	if (DevSettingsCDO->TagReadableNames.Contains(GetSourceItemCDO()->SlotTag))
	{
		TagName = DevSettingsCDO->TagReadableNames[GetSourceItemCDO()->SlotTag];
	}
	
	//Preview: Lvl 4 Head
	return FText::Format(
	FText::FromString(TEXT("Lvl {0} {1}")), 
	FText::AsNumber(GetItemLevel() + 1), 
	TagName);
}
