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

void UBSEquipmentEffect::PostLoad()
{
	Super::PostLoad();
	
	if (SlotTag.IsValid())
	{
		CachedAssetTags.AddTag(SlotTag);
	}
}

void UBSEquipmentEffect::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (SlotTag.IsValid())
	{
		CachedAssetTags.AddTag(SlotTag);
	}

}

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
	
	return Super::IsDataValid(Context);
}

#endif
