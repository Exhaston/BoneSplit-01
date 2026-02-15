// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/HUD/BSFloatingNamePlate.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Widgets/HUD/BSAttributeBar.h"

void UBSFloatingNamePlate::InitializeAbilitySystemComponent(UAbilitySystemComponent* InAsc)
{
	AbilitySystemComponent = InAsc;
	HealthBarWidget->InitializeAttributeBar(AbilitySystemComponent);
	SetLocation();
}

void UBSFloatingNamePlate::SetLocation()
{
	if (AbilitySystemComponent && AbilitySystemComponent->GetAvatarActor() && GetOwningPlayer())
	{
		ACharacter* Character = Cast<ACharacter>(AbilitySystemComponent->GetAvatarActor());
		
		FVector Origin = Character ? Character->GetMesh()->GetComponentLocation() + 
			FVector::UpVector * Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : 
		AbilitySystemComponent->GetAvatarActor()->GetActorLocation();
		
		float OffsetZ = Character ? Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 1.25 : 150;
		
		FVector2D ScreenLocation;
		if (GetOwningPlayer()->ProjectWorldLocationToScreen(Origin + FVector::UpVector * OffsetZ, ScreenLocation, true))
		{
			
			const FVector2D DesiredSize = GetDesiredSize();

			const FVector2D CenteredPosition = ScreenLocation - (DesiredSize * 0.5f);
			SetPositionInViewport(CenteredPosition, true);
			SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
