// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/MainMenu/BSMenuController.h"

#include "GameInstance/BSLoadingScreenSubsystem.h"
#include "Widgets/MainMenu/BSMainMenuHud.h"

void ABSMenuController::BeginPlay()
{
	Super::BeginPlay();
	GetHUD<ABSMainMenuHud>()->NotifyReady();
}

void ABSMenuController::PreClientTravel(const FString& PendingURL, const ETravelType TravelType, const bool bIsSeamlessTravel)
{
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
	
	/*
	if (UBSLoadingScreenSubsystem* LoadingScreenSubsystem = GetGameInstance()->GetSubsystem<UBSLoadingScreenSubsystem>(); GetLocalPlayer() && GetLocalPlayer()->IsPrimaryPlayer())
	{
		LoadingScreenSubsystem->AddLoadingScreen(this);
	}
	*/
}
