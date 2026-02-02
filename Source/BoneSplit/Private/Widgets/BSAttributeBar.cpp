// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/BSAttributeBar.h"

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "CommonTextBlock.h"
#include "Components/Image.h"

void UBSAttributeBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (HealthBarImage && HealthBarImage->GetDynamicMaterial())
	{
		HealthBarImage->GetDynamicMaterial()->SetVectorParameterValue("Color", Color);
	}
}

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
	
	CurrentValue = AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute);
	CurrentMaxValue = AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute);
	
	if (CurrentMaxValue > 0)
	{
		Current = CurrentValue / CurrentMaxValue;
		SmoothedCurrent = Current;
		UpdateBar(-1);
		UpdateText();
	}
	
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent.Get()->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).AddWeakLambda(
		this, [this](const FOnAttributeChangeData& Data)
		{
			CurrentMaxValue = AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute);
			UpdateText();
			if (CurrentMaxValue > 0)
			{
				CurrentValue = AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute);
				Current = CurrentValue / CurrentMaxValue;
			}

		});
		AbilitySystemComponent.Get()->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddWeakLambda(
		this, [this](const FOnAttributeChangeData& Data)
		{
			CurrentMaxValue = AbilitySystemComponent.Get()->GetNumericAttribute(MaxAttribute);
			UpdateText();
			if (CurrentMaxValue > 0)
			{
				CurrentValue = AbilitySystemComponent.Get()->GetNumericAttribute(CurrentAttribute);
				Current = CurrentValue / CurrentMaxValue;
			}
		});
	}
	
	if (HealthBarImage->GetDynamicMaterial())
	{
		HealthBarImage->GetDynamicMaterial()->SetVectorParameterValue("Color", Color);
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
		HealthBarImage->GetDynamicMaterial()->SetScalarParameterValue("Previous", SmoothedCurrent);
	}
}

void UBSAttributeBar::UpdateText()
{
	HealthBarImage->GetDynamicMaterial()->SetScalarParameterValue("Current", Current);
	const FString CurrentNum = FString::FromInt(FMath::RoundToInt(CurrentValue));
	const FString CurrentMaxNum = FString::FromInt(FMath::RoundToInt(CurrentMaxValue));
	HealthBarText->SetText(FText::FromString(CurrentNum + " / " + CurrentMaxNum));
}
