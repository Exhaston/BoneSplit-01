// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/CharacterWidgets/BSAttributeText.h"

#include "AbilitySystemComponent.h"
#include "CommonTextBlock.h"
#include "Actors/Player/BSPlayerState.h"

void UBSAttributeText::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (Attribute.IsValid())
	{
		StatText->SetText(FText::FromString("<" + Attribute.AttributeName + "> *" + FString::SanitizeFloat(Multiplier) + Suffix));
	}
	
}

void UBSAttributeText::NativeConstruct()
{
	Super::NativeConstruct();
	
	const ABSPlayerState* PS = GetOwningPlayerState<ABSPlayerState>();
	check(PS);
	
	UAbilitySystemComponent* Asc = PS->GetAbilitySystemComponent();
	
	SetAttributeText();

	Asc->GetGameplayAttributeValueChangeDelegate(Attribute).AddWeakLambda(
		this, [this](const FOnAttributeChangeData& Data)
	{
			SetAttributeText();
	});
}

void UBSAttributeText::NativeDestruct()
{
	if (IsValid(this))
	{
		if (const ABSPlayerState* PS = GetOwningPlayerState<ABSPlayerState>()) //Might not be valid if quitting editor etc.
		{
			UAbilitySystemComponent* Asc = PS->GetAbilitySystemComponent();

			if (Asc)
			{
				Asc->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
			}
		}
	}

	
	Super::NativeDestruct();
}

void UBSAttributeText::SetAttributeText()
{
	const ABSPlayerState* PS = GetOwningPlayerState<ABSPlayerState>();
	check(PS);
	
	UAbilitySystemComponent* Asc = PS->GetAbilitySystemComponent();
	
	float Value = FMath::RoundToInt(Asc->GetNumericAttribute(Attribute) * Multiplier);
	
	FString DisplayValue = FString::FromInt(Value);
	
	StatText->SetText(FText::FromString(DisplayValue + Suffix));
}