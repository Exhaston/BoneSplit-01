// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/BSAttributeBar.h"

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Components/Image.h"

void UBSAttributeBar::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (bPlayerAsc)
	{
		bool bFoundAsc = false;
	
		if (GetOwningPlayerPawn())
		{
			if (const IAbilitySystemInterface* PawnInterface = Cast<IAbilitySystemInterface>(GetOwningPlayerPawn()))
			{
				AbilitySystemComponent = PawnInterface->GetAbilitySystemComponent();
				bFoundAsc = true;
			}
		}
	
		if (APlayerState* PS = GetOwningPlayerState(); PS && !bFoundAsc)
		{
			if (const IAbilitySystemInterface* PlayerStateInterface = Cast<IAbilitySystemInterface>(PS))
			{
				AbilitySystemComponent = PlayerStateInterface->GetAbilitySystemComponent();
				bFoundAsc = true;
			}
		}
	}
	
	float CurrentAttributeValue = AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute);
	float MaxAttributeValue = AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute);
	
	if (MaxAttributeValue > 0)
	{
		Current = CurrentAttributeValue / MaxAttributeValue;
		SmoothedCurrent = Current;
		UpdateBar(-1);
	}
	
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent.Get()->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).AddWeakLambda(
		this, [this](const FOnAttributeChangeData& Data)
		{
			float MaxAttributeValue = AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute);
			
			if (MaxAttributeValue > 0)
			{
				Current = AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute) / MaxAttributeValue;
			}

		});
		AbilitySystemComponent.Get()->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddWeakLambda(
		this, [this](const FOnAttributeChangeData& Data)
		{
			float MaxAttributeValue = AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute);
			
			if (MaxAttributeValue > 0)
			{
				Current = AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute) / MaxAttributeValue;
			}
		});
	}
}

void UBSAttributeBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (IsValid(this))
	{
		if (Current != SmoothedCurrent)
		{
			UpdateBar(InDeltaTime);
		}
	}
}

void UBSAttributeBar::UpdateBar(const float DeltaTime)
{
	if (AbilitySystemComponent.IsValid() && CurrentAttribute.IsValid() && MaxAttribute.IsValid())
	{
		SmoothedCurrent = FMath::FInterpTo(SmoothedCurrent, Current, DeltaTime, InterpSpeed);
		HealthBarImage->GetDynamicMaterial()->SetScalarParameterValue("Current", Current);
		HealthBarImage->GetDynamicMaterial()->SetScalarParameterValue("Previous", SmoothedCurrent);
	}
}
