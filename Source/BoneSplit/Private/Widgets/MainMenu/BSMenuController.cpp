// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/MainMenu/BSMenuController.h"

#include "GameInstance/BSLoadingScreenSubsystem.h"
#include "Widgets/BSLocalWidgetSubsystem.h"

void ABSMenuController::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalController())
	{
		UBSLocalWidgetSubsystem::GetWidgetSubsystem(this)->CreatePlayerUI(this, true);
	}
}

void ABSMenuController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
	if (UBSLoadingScreenSubsystem* LoadingScreenSubsystem = GetLocalPlayer()->GetSubsystem<UBSLoadingScreenSubsystem>(); GetLocalPlayer()->IsPrimaryPlayer())
	{
		LoadingScreenSubsystem->AddLoadingScreen(nullptr);
	}
}
