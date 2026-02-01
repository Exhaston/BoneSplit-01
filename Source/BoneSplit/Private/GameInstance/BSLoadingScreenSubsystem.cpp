// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameInstance/BSLoadingScreenSubsystem.h"

#include "CommonLazyImage.h"
#include "Blueprint/UserWidget.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "GameState/BSTravelManager.h"
#include "Widgets/BSLoadingScreenWidget.h"

UBSLoadingScreenSubsystem::UBSLoadingScreenSubsystem()
{

}

void UBSLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	/*
	FCoreUObjectDelegates::PreLoadMap.AddWeakLambda(this, [this]
		(const FString& Dest)
	{
		if (GetLocalPlayer()->IsPrimaryPlayer())
		{
			AddLoadingScreen(nullptr);
		}
	});
	*/
}

void UBSLoadingScreenSubsystem::StartTravel(const TSubclassOf<UBSMapData> MapData, FString TravelDestTag)
{
	const UBSMapData* MapDataCDO = GetDefault<UBSMapData>(MapData);
	
	FString TravelDest = MapDataCDO->Map.GetAssetName();

	/*
	if (!GetWorld()->IsNetMode(NM_ListenServer) && GetPartyMode() != EBSPartyMode::Party_Private)
	{
		TravelDest += "?Listen";
	}
	*/
	
	CurrentTravelTag = TravelDestTag;
	
	GetWorld()->ServerTravel(TravelDest);
}

void UBSLoadingScreenSubsystem::AddLoadingScreen(const UBSMapData* MapData)
{
	check(GEngine);
	if (IsRunningDedicatedServer()) return;
	const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
	
	if (!LoadingScreenWidgetIns.IsValid() && !DeveloperSettings->LoadingScreenWidget.IsNull() && GetLocalPlayer())
	{
		if (UGameViewportClient* Viewport = GEngine->GameViewport.Get())
		{
			if (UBSLoadingScreenWidget* LoadingWidget = CreateWidget<UBSLoadingScreenWidget>(
			GetLocalPlayer<ULocalPlayer>()->GetGameInstance(),
			DeveloperSettings->LoadingScreenWidget.LoadSynchronous()))
			{
				LoadingScreenWidgetIns = LoadingWidget->TakeWidget();
				Viewport->AddViewportWidgetContent(LoadingScreenWidgetIns.ToSharedRef());
			}
		}
	}
}

void UBSLoadingScreenSubsystem::RemoveLoadingScreen()
{
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
