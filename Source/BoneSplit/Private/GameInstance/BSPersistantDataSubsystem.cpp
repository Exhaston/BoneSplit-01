// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameInstance/BSPersistantDataSubsystem.h"

#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "Actors/Player/BSSaveGame.h"
#include "Blueprint/UserWidget.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "GameState/BSTravelManager.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/BSLoadingScreenWidget.h"

UBSPersistantDataSubsystem* UBSPersistantDataSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	const UGameInstance* GameInstance = World->GetGameInstanceChecked<UGameInstance>();
	return GameInstance->GetSubsystem<UBSPersistantDataSubsystem>();
}

void UBSPersistantDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
	SaveGameClass = DeveloperSettings->SaveGameClass.LoadSynchronous();
}

void UBSPersistantDataSubsystem::StartTravel(const TSubclassOf<UBSMapData> MapData, FString TravelDestTag)
{
	const UBSMapData* MapDataCDO = GetDefault<UBSMapData>(MapData);
	
	FString TravelDest = MapDataCDO->Map.GetAssetName();

	if (!GetWorld()->IsNetMode(NM_ListenServer) && GetPartyMode() != EBSPartyMode::Party_Private)
	{
		TravelDest += "?Listen";
	}
	
	CurrentTravelTag = TravelDestTag;
	
	GetWorld()->ServerTravel(TravelDest);
}

void UBSPersistantDataSubsystem::AddLoadingScreen(const UBSMapData* MapData)
{
	OnMapLoadBegin.Broadcast(MapData);
	if (GetWorld()->GetNetMode() != NM_DedicatedServer) //Dedicated servers doesn't need any UI elements.
	{
		const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
		
		if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport();
			Viewport && !DeveloperSettings->LoadingScreenWidget.IsNull())
		{
			if (UBSLoadingScreenWidget* LoadingWidget = CreateWidget<UBSLoadingScreenWidget>(
				GetGameInstance(),DeveloperSettings->LoadingScreenWidget.LoadSynchronous()))
			{

				LoadingWidget->MapName->SetText(MapData->MapName);
				LoadingWidget->SplashImage->SetBrushFromLazyTexture(MapData->MapSplashTexture);
				LoadingScreenWidgetIns = LoadingWidget->TakeWidget();
				Viewport->AddViewportWidgetContent(LoadingScreenWidgetIns.ToSharedRef());
			}
		}
	}
}

void UBSPersistantDataSubsystem::RemoveLoadingScreen()
{
	OnMapLoadEnd.Broadcast();
	
	if (LoadingScreenWidgetIns.IsValid())
	{
		if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
		{
			Viewport->RemoveViewportWidgetContent(LoadingScreenWidgetIns.ToSharedRef());
		}
	}
}

FString UBSPersistantDataSubsystem::GetTravelDestTag() const
{
	return CurrentTravelTag;
}

EBSPartyMode UBSPersistantDataSubsystem::GetPartyMode() const
{
	return PartyMode;
}

UBSSaveGame* UBSPersistantDataSubsystem::GetOrLoadSaveGame(const APlayerController* PC)
{
	const FString SaveSlotName = "DefaultSave";
	if (SaveGameInstance)
	{
		return SaveGameInstance; //Fast already cached save object loaded into memory
	}
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, PC->GetLocalPlayer()->GetPlatformUserIndex()))
	{
		//We can load a valid save into memory
		SaveGameInstance = Cast<UBSSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 
		PC->GetLocalPlayer()->GetPlatformUserIndex()));
	}
	else
	{     
		//No saves found, creating a new one as a fallback...
		SaveGameInstance = Cast<UBSSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	}
	
	return SaveGameInstance;
}
