// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "BSDestroySessionProxy.h"

#include "BSJoinSessionCallbackProxyAdvanced.h"

UBSDestroySessionProxy::UBSDestroySessionProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
{
}

UBSDestroySessionProxy* UBSDestroySessionProxy::DestroySession(UObject* WorldContextObject, class APlayerController* PlayerController)
{
	UBSDestroySessionProxy* Proxy = NewObject<UBSDestroySessionProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UBSDestroySessionProxy::Activate()
{
	FBSOnlineSubsystemBPCallHelper Helper(TEXT("DestroySession"), WorldContextObject);
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			DelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(Delegate);
			Sessions->DestroySession(NAME_GameSession);

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

void UBSDestroySessionProxy::OnCompleted(FName SessionName, bool bWasSuccessful)
{
	FBSOnlineSubsystemBPCallHelper Helper(TEXT("DestroySessionCallback"), WorldContextObject);
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(DelegateHandle);
		}
	}


	if (bWasSuccessful)
	{
		OnSuccess.Broadcast();
	}
	else
	{
		OnFailure.Broadcast();
	}
}