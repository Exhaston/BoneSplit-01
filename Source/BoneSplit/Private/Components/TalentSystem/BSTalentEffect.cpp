// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/TalentSystem/BSTalentEffect.h"

#include "GameplayEffect.h"

#if WITH_EDITOR

#include "Misc/DataValidation.h"

bool UBSTalentEffect::CanEditChange(const FProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSTalentEffect, StackingType))
	{
		return false;
	}
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSTalentEffect, DurationPolicy))
	{
		return false;
	}
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSTalentEffect, DurationMagnitude))
	{
		return false;
	}
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSTalentEffect, Period))
	{
		return false;
	}
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UBSTalentEffect, PeriodicInhibitionPolicy))
	{
		return false;
	}
	return ParentVal;
}

EDataValidationResult UBSTalentEffect::IsDataValid(FDataValidationContext& Context) const
{
	if (DurationPolicy != EGameplayEffectDurationType::Infinite)
	{
		Context.AddError(
			FText::FromString("Detected Gameplay Effect with non infinite duration, "
					 "for talents the duration policy must be infinite."));
	}
	return Super::IsDataValid(Context);
}

#endif

UBSTalentEffect::UBSTalentEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackLimitCount = 3;
}



int32 UBSTalentEffect::GetMaxLevel() const
{
	return GetStackLimitCount();
}

TSoftObjectPtr<UTexture2D> UBSTalentEffect::GetIcon_Implementation() const
{
	return TalentIcon;
}

FLinearColor UBSTalentEffect::GetTint_Implementation() const
{
	return FLinearColor::White;
}
