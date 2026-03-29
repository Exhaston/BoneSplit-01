// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSAttributeText.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CommonTextBlock.h"

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
	
	IAbilitySystemInterface* AscI = GetOwningPlayer<IAbilitySystemInterface>();
	
	UAbilitySystemComponent* Asc = AscI->GetAbilitySystemComponent();
	
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
		IAbilitySystemInterface* AscI = GetOwningPlayer<IAbilitySystemInterface>();
	
		UAbilitySystemComponent* Asc = AscI->GetAbilitySystemComponent();
		Asc->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
	}

	
	Super::NativeDestruct();
}

void UBSAttributeText::SetAttributeText()
{
	IAbilitySystemInterface* AscI = GetOwningPlayer<IAbilitySystemInterface>();
	UAbilitySystemComponent* Asc = AscI->GetAbilitySystemComponent();
	
	float Value = FMath::RoundToInt(Asc->GetNumericAttribute(Attribute) * Multiplier);
	
	FString DisplayValue = FString::FromInt(Value);
	
	StatText->SetText(FText::FromString(DisplayValue + Suffix));
}