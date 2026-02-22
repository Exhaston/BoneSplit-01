// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameSystems/BSGameManagerSubsystem.h"

#include "BSDestroySessionProxy.h"
#include "GameFramework/GameModeBase.h"
#include "GameSettings/BSDeveloperSettings.h"

UE_DEFINE_GAMEPLAY_TAG(ActorClass, "ActorClass");

void UBSGameManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
	HubLevel = DeveloperSettings->HubLevel;
	MainMenuLevel = DeveloperSettings->MainMenuLevel;

	for (auto& OverrideClass : DeveloperSettings->ActorOverrideClasses)
	{
		if (!OverrideClass.Key.IsValid()) continue;
		if (OverrideClass.Value.IsNull()) continue;
		if (UClass* LoadedClass = OverrideClass.Value.LoadSynchronous())
		{
			ActorOverrideClasses.Add({OverrideClass.Key, LoadedClass} );
		}
	}
}

UBSGameManagerSubsystem* UBSGameManagerSubsystem::Get(const UObject* WorldContext)
{
	return GEngine->GetWorldFromContextObjectChecked(WorldContext)->GetGameInstance()->GetSubsystem<UBSGameManagerSubsystem>();
}

void UBSGameManagerSubsystem::InitializeHost()
{
	GetGameInstance()->GetWorld()->ServerTravel(HubLevel.GetAssetName() + "?Listen", true);
}

void UBSGameManagerSubsystem::InitializeSinglePlayer()
{
	GetGameInstance()->GetWorld()->ServerTravel(HubLevel.GetAssetName(), true);
}

void UBSGameManagerSubsystem::QuitToMenu()
{
	if (GetGameInstance()->GetWorld()->GetAuthGameMode() && GetGameInstance()->GetWorld()->GetNetMode() == NM_ListenServer)
	{
		GetGameInstance()->GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = false;
		UBSDestroySessionProxy* DestroySessionCallbackProxy = UBSDestroySessionProxy::DestroySession(GetGameInstance(), GetGameInstance()->GetPrimaryPlayerController());
		DestroySessionCallbackProxy->OnSuccess.AddDynamic(this, &UBSGameManagerSubsystem::OnSessionEnd);
		DestroySessionCallbackProxy->OnFailure.AddDynamic(this, &UBSGameManagerSubsystem::OnSessionEnd);
		DestroySessionCallbackProxy->Activate();
	}
	else
	{
		OnSessionEnd();
	}
}

void UBSGameManagerSubsystem::OnSessionEnd()
{
	GetGameInstance()->GetWorld()->ServerTravel(MainMenuLevel.GetAssetName(), true);
}

void UBSGameManagerSubsystem::TravelToMap(const TSoftObjectPtr<UWorld> InMapAsset, const FString InPlayerSpawnTag)
{
	if (!GetGameInstance()) return;
	
	SetTravelTagDestination(InPlayerSpawnTag);
	
	GetGameInstance()->GetWorld()->ServerTravel(InMapAsset.GetAssetName(), false);
}


