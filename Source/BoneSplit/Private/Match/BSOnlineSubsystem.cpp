// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Match/BSOnlineSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Match/BSOnlineSettings.h"

static const FName SESSION_NAME = TEXT("BSGameSession");

void UBSOnlineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (const IOnlineSubsystem* Oss = Online::GetSubsystem(GetWorld()))
    {
        GetSessionInterface() = Oss->GetSessionInterface();
    }
}

void UBSOnlineSubsystem::Deinitialize()
{
    // Clean up any lingering delegate registrations
    if (GetSessionInterface().IsValid())
    {
        GetSessionInterface()->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
        GetSessionInterface()->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
    }
    Super::Deinitialize();
}

// ── Public API ───────────────────────────────────────────────────────────────

void UBSOnlineSubsystem::StartGame(bool bHosting)
{
    if (bHosting)
    {
        StartHosting();
    }
    else
    {
        // Pure offline singleplayer — skip Steam entirely
        UGameplayStatics::OpenLevel(GetWorld(), *GetBSOnlineSettings()->GameStartMap.GetAssetName(), true);
        OnGameBeginDelegate.Broadcast();
    }
}

void UBSOnlineSubsystem::StartHosting()
{
    if (!GetSessionInterface().IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("BSOnlineSubsystem: No session interface — falling back to offline."));
        StartGame(false);
        return;
    }

    // Destroy any existing session first (safe to call even if none exists)
    if (GetSessionInterface()->GetNamedSession(SESSION_NAME))
    {
        OnDestroySessionCompleteDelegateHandle = GetSessionInterface()->AddOnDestroySessionCompleteDelegate_Handle(
            FOnDestroySessionCompleteDelegate::CreateLambda([this](FName Name, bool bOK)
            {
                GetSessionInterface()->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
                // Retry hosting now that the old session is gone
                StartHosting();
            }));
        GetSessionInterface()->DestroySession(SESSION_NAME);
        return;
    }

    // Build session settings
    const TSharedPtr<FOnlineSessionSettings> Settings = MakeShareable(new FOnlineSessionSettings());
    Settings->bIsLANMatch           = Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL" || bLanMatch;
    Settings->NumPublicConnections  = GetBSOnlineSettings()->MaxPlayers;
    Settings->bShouldAdvertise      = true;
    Settings->bAllowJoinInProgress  = false;
    Settings->bUsesPresence         = true;
    Settings->bUseLobbiesIfAvailable = true;

    OnCreateSessionCompleteDelegateHandle =  GetSessionInterface()->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UBSOnlineSubsystem::OnCreateSessionComplete));

    const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
    if (! GetSessionInterface()->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), SESSION_NAME, *Settings))
    {
         GetSessionInterface()->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
        UE_LOG(LogTemp, Error, TEXT("BSOnlineSubsystem: CreateSession call failed immediately."));
    }
}

void UBSOnlineSubsystem::EndHosting()
{
    bIsHosting = false;
    LeaveGame();
}

void UBSOnlineSubsystem::LeaveGame()
{
    if (! GetSessionInterface().IsValid() || ! GetSessionInterface()->GetNamedSession(SESSION_NAME))
    {
        UGameplayStatics::OpenLevel(GetWorld(), *GetBSOnlineSettings()->MainMenuMap.GetAssetName(), true, TEXT(""));
        OnGameLeftDelegate.Broadcast();
        return;
    }

    OnDestroySessionCompleteDelegateHandle =  GetSessionInterface()->AddOnDestroySessionCompleteDelegate_Handle(
        FOnDestroySessionCompleteDelegate::CreateUObject(this, &UBSOnlineSubsystem::OnDestroySessionComplete));

    GetSessionInterface()->DestroySession(SESSION_NAME);
}

UWorld* UBSOnlineSubsystem::GetWorld() const
{
    return GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
}

// ── Private callbacks ────────────────────────────────────────────────────────

void UBSOnlineSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    GetSessionInterface()->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

    if (!bWasSuccessful)
    {
        UE_LOG(LogTemp, Error, TEXT("BSOnlineSubsystem: CreateSession failed for '%s'."), *SessionName.ToString());
        return;
    }

    bIsHosting = true;

    // Now it's safe to open the level as a listen server
    UGameplayStatics::OpenLevel(GetWorld(), *GetBSOnlineSettings()->GameStartMap.GetAssetName(), true, TEXT("?listen"));

    OnGameBeginDelegate.Broadcast();
}

void UBSOnlineSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    GetSessionInterface()->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

    if (!bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("BSOnlineSubsystem: DestroySession failed for '%s'."), *SessionName.ToString());
    }

    // Travel back regardless — Steam session may have already timed out
    UGameplayStatics::OpenLevel(GetWorld(), *GetBSOnlineSettings()->MainMenuMap.GetAssetName(), true, TEXT(""));
    OnGameLeftDelegate.Broadcast();
}

IOnlineSessionPtr UBSOnlineSubsystem::GetSessionInterface() const
{
    const IOnlineSubsystem* Oss = Online::GetSubsystem(GetWorld());
    if (!Oss) return nullptr;
    return Oss->GetSessionInterface();
}