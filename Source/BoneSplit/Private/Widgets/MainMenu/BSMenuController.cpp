// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/MainMenu/BSMenuController.h"

#include "Camera/CameraActor.h"
#include "GameInstance/BSLoadingScreenSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/MainMenu/BSMainMenuHud.h"

void ABSMenuController::BeginPlay()
{
	Super::BeginPlay();
	GetHUD<ABSMainMenuHud>()->NotifyReady();
	
	ACameraActor* CameraActor = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass()));
	SetViewTarget(CameraActor);
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
