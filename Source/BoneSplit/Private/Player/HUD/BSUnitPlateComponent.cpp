// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/HUD/BSUnitPlateComponent.h"

#include "Player/HUD/BSWidget_UnitPlate.h"


UBSUnitPlateComponent::UBSUnitPlateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0;
	Space = EWidgetSpace::Screen;
	SetDrawAtDesiredSize(true);
}


void UBSUnitPlateComponent::SetupUnitPlate(UAbilitySystemComponent* InAbilitySystemComponent, const FText UnitName)
{
	check(GetWidget());
	if (UBSWidget_UnitPlate* UnitPlate = Cast<UBSWidget_UnitPlate>(GetWidget()))
	{
		UnitPlate->SetOwningAsc(InAbilitySystemComponent);
		if (!UnitName.IsEmptyOrWhitespace())
		{
			UnitPlate->SetUnitName(UnitName);
		}
	}
}

void UBSUnitPlateComponent::SetActive(bool bNewActive, bool bReset)
{
	Super::SetActive(bNewActive, bReset);
	bShouldBeVisible = bNewActive;
}

void UBSUnitPlateComponent::BeginPlay()
{
	Super::BeginPlay();
	SetVisibility(false);
}

void UBSUnitPlateComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!GetOwnerPlayer() || !GetOwner() || !GetWidget()) return;
	
	CurrentUpdate += DeltaTime;
	
	if (CurrentUpdate < VisibilityUpdateInterval)
	{
		return;
	}
	
	CurrentUpdate = 0;
	
	if (const APlayerController* OwnerPlayerController = GetOwnerPlayer()->GetPlayerController(GetWorld()))
	{
		FVector EyeOrigin;
		FRotator EyeRotation;
		
		OwnerPlayerController->GetPlayerViewPoint(EyeOrigin, EyeRotation);
		
		const bool bLineOfSight = !bRequireVisibility || OwnerPlayerController->LineOfSightTo(GetOwner(), EyeOrigin);
		SetVisibility(bLineOfSight && FVector::Distance(EyeOrigin, GetOwner()->GetActorLocation()) <= VisibilityDistance && bShouldBeVisible);
	}
}

