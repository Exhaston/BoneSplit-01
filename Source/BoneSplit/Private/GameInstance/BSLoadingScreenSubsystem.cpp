// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameInstance/BSLoadingScreenSubsystem.h"

#include "CommonLazyImage.h"
#include "Blueprint/UserWidget.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "Widgets/Base/BSLoadingScreenWidget.h"

UBSLoadingScreenSubsystem::UBSLoadingScreenSubsystem()
{

}

void UBSLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UBSLoadingScreenSubsystem::AddLoadingScreen(APlayerController* LocalController)
{
	check(GEngine);
	if (IsRunningDedicatedServer()) return;
	if (!LocalController->IsPrimaryPlayer()) return;
	const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
	
	/*
	if (!LoadingScreenWidgetIns.IsValid())
	{
		if (UGameViewportClient* Viewport = GEngine->GameViewport.Get())
		{
			if (UBSLoadingScreenWidget* LoadingWidget = CreateWidget<UBSLoadingScreenWidget>(
			GetGameInstance(),
			DeveloperSettings->LoadingScreenWidget.LoadSynchronous()))
			{
				LoadingScreenWidgetIns = LoadingWidget->TakeWidget();
				Viewport->AddViewportWidgetContent(LoadingScreenWidgetIns.ToSharedRef());
			}
		}
	}
	*/
}

void UBSLoadingScreenSubsystem::RemoveLoadingScreen(APlayerController* LocalController)
{
	if (!LocalController->IsPrimaryPlayer()) return;
	OnMapLoadEnd.Broadcast();
	
	if (LoadingScreenWidgetIns.IsValid())
	{
		if (UGameViewportClient* Viewport = GEngine->GameViewport.Get())
		{
			Viewport->RemoveViewportWidgetContent(LoadingScreenWidgetIns.ToSharedRef());
			LoadingScreenWidgetIns.Reset();
		}
	}
}
