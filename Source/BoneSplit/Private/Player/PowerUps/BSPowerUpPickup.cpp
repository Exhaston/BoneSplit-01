// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/PowerUps/BSPowerUpPickup.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "Player/PowerUps/BSWidget_PowerUpSelector.h"
#include "Player/BSPlayerStateBase.h"


ABSPowerUpPickup::ABSPowerUpPickup(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABSPowerUpPickup::OnInteract_Implementation(AActor* InstigatorInteractor)
{
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(InstigatorInteractor))
	{
		if (const ABSPlayerStateBase* PS = OwnerCharacter->GetPlayerState<ABSPlayerStateBase>(); PS && PS->GetPlayerController())
		{
			UBSWidget_PowerUpSelector* PowerUpSelector = CreateWidget<UBSWidget_PowerUpSelector>(PS->GetPlayerController(), PowerUpSelectorClass);
			PowerUpSelector->InitializeSelector(ChoiceData);
			PowerUpSelector->AddToPlayerScreen();
			Destroy();
		}
	}
}

