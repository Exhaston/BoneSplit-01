// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "BSJoinSessionCallbackProxyAdvanced.h"

#include "FindSessionsCallbackProxy.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"


UBSJoinSessionCallbackProxyAdvanced::UBSJoinSessionCallbackProxyAdvanced(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
{
}

FBSOnlineSubsystemBPCallHelper::FBSOnlineSubsystemBPCallHelper(const TCHAR* CallFunctionContext, UObject* WorldContextObject, FName SystemName)
	: OnlineSub(Online::GetSubsystem(GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull), SystemName))
	, FunctionContext(CallFunctionContext)
{
	if (OnlineSub == nullptr)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s - Invalid or uninitialized OnlineSubsystem"), FunctionContext), ELogVerbosity::Warning);
	}
}

void FBSOnlineSubsystemBPCallHelper::QueryIDFromPlayerController(APlayerController* PlayerController)
{
	UserID.Reset();

	APlayerState* PlayerState = nullptr;
	if (PlayerController != nullptr)
	{
		PlayerState = ToRawPtr(PlayerController->PlayerState);
	}

	if (PlayerState != nullptr)
	{
		UserID = PlayerState->GetUniqueId().GetUniqueNetId();
		if (!UserID.IsValid())
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s - Cannot map local player to unique net ID"), FunctionContext), ELogVerbosity::Warning);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s - Invalid player state"), FunctionContext), ELogVerbosity::Warning);
	}
}

UBSJoinSessionCallbackProxyAdvanced* UBSJoinSessionCallbackProxyAdvanced::JoinSession(UObject* WorldContextObject, class APlayerController* PlayerController, const FBlueprintSessionResult& SearchResult)
{
	UBSJoinSessionCallbackProxyAdvanced* Proxy = NewObject<UBSJoinSessionCallbackProxyAdvanced>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->OnlineSearchResult = SearchResult.OnlineResult;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UBSJoinSessionCallbackProxyAdvanced::Activate()
{
	FBSOnlineSubsystemBPCallHelper Helper(TEXT("JoinSession"), WorldContextObject);
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			DelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(Delegate);
			Sessions->JoinSession(*Helper.UserID, NAME_GameSession, OnlineSearchResult);

			// OnCompleted will get called, nothing more to do now
			return;
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("Sessions not supported by Online Subsystem"), ELogVerbosity::Warning);
		}
	}

	// Fail immediately
	OnFailure.Broadcast();
}

void UBSJoinSessionCallbackProxyAdvanced::OnCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	FBSOnlineSubsystemBPCallHelper Helper(TEXT("JoinSessionCallback"), WorldContextObject);
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(DelegateHandle);

			if (Result == EOnJoinSessionCompleteResult::Success)
			{
				// Client travel to the server
				FString ConnectString;
				if (Sessions->GetResolvedConnectString(NAME_GameSession, ConnectString) && PlayerControllerWeakPtr.IsValid())
				{
					UE_LOG_ONLINE_SESSION(Log, TEXT("Join session: traveling to %s"), *ConnectString);
					PlayerControllerWeakPtr->ClientTravel(ConnectString, TRAVEL_Absolute);
					OnSuccess.Broadcast();
					return;
				}
			}
		}
	}

	OnFailure.Broadcast();
}
