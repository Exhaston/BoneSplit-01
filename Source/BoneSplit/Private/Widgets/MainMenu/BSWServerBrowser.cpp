// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/MainMenu/BSWServerBrowser.h"

#include "BSJoinSessionCallbackProxyAdvanced.h"
#include "CommonLazyImage.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "FindSessionsCallbackProxyAdvanced.h"
#include "Widgets/MainMenu/BSWServerButton.h"
#include "BoneSplit/BoneSplit.h"
#include "GameSystems/BSGameManagerSubsystem.h"
#include "Widgets/BSLocalWidgetSubsystem.h"
#include "Widgets/Base/BSToggleButton.h"
#include "Widgets/Base/BSWButtonBase.h"

static int32 MaxSessionPlayers = 4;

void UBSWServerBrowser::NativeConstruct()
{
	Super::NativeConstruct();
	SearchLanButton->SetState(bEnableLan);
	SearchLanButton->OnClicked().AddWeakLambda(this, [this]()
	{
		bEnableLan = !bEnableLan;
		SearchLanButton->SetState(bEnableLan);
	});
	
	PrivateToggleButton->SetState(bPrivateSession);
	PrivateToggleButton->OnClicked().AddWeakLambda(this, [this]()
	{
		bPrivateSession = !bPrivateSession;
		PrivateToggleButton->SetState(bPrivateSession);
	});
	
	RefreshButton->OnClicked().AddWeakLambda(this, [this]()
	{
		StartSearch();
	});
	
	HostButton->OnClicked().AddWeakLambda(this, [this]()
	{
		const TArray<FSessionPropertyKeyPair> Filters;
		
		UCreateSessionCallbackProxyAdvanced* SessionCallbackProxy = 
			UCreateSessionCallbackProxyAdvanced::CreateAdvancedSession(
			GetOwningPlayer(), 
			Filters, 
			GetOwningPlayer(),
			bPrivateSession ? 0 : MaxSessionPlayers, 
			bPrivateSession ? MaxSessionPlayers : 0, 
			bEnableLan, 
			true, 
			false, 
			true, 
			true, 
			false, 
			false, 
			false, 
			true, 
			false, 
			true);
		
		SessionCallbackProxy->OnSuccess.AddDynamic(this, &UBSWServerBrowser::OnHostComplete);
		SessionCallbackProxy->Activate();
	});
}

void UBSWServerBrowser::NativeOnActivated()
{
	Super::NativeOnActivated();

	StartSearch();
}

void UBSWServerBrowser::StartSearch()
{
	FoundSessions.Empty();
	SessionPanel->ClearChildren();
	SearchingImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	const TArray<FSessionsSearchSetting> Filters;
	
	UFindSessionsCallbackProxyAdvanced* FindSessionsCallbackProxy =
		UFindSessionsCallbackProxyAdvanced::FindSessionsAdvanced(
		GetOwningPlayer(), 
		GetOwningPlayer(), 
		1000, 
		bEnableLan,
		EBPServerPresenceSearchType::AllServers, 
		Filters, 
		false, 
		false, 
		false, 
		0);
	
	FindSessionsCallbackProxy->OnSuccess.AddDynamic(this, &UBSWServerBrowser::OnSessionsFound);
	FindSessionsCallbackProxy->OnFailure.AddDynamic(this, &UBSWServerBrowser::OnSessionFindFailure);
	
	FindSessionsCallbackProxy->Activate();
}

void UBSWServerBrowser::JoinSessionAt(const int32 Index)
{
	const FBlueprintSessionResult& SelectedSession = FoundSessions[Index];
	
	if (!SelectedSession.OnlineResult.IsValid())
	{                                                                                                 
		UE_LOG(BoneSplit, Display, TEXT("Session is invalid. Could have changed since last refresh."));
		return;
	}
	
	UE_LOG(BoneSplit, Display, TEXT("%s"), 
		*FString("Attempting to join Session" + SelectedSession.OnlineResult.Session.OwningUserName));
	
	UBSJoinSessionCallbackProxyAdvanced* JoinSessionCallbackProxy = UBSJoinSessionCallbackProxyAdvanced::JoinSession(
	GetOwningLocalPlayer(), GetOwningPlayer(), FoundSessions[Index]);
	JoinSessionCallbackProxy->OnFailure.AddDynamic(this, &UBSWServerBrowser::OnJoinFailed);
	JoinSessionCallbackProxy->Activate();
}

void UBSWServerBrowser::OnJoinFailed()
{
	UE_LOG(BoneSplit, Display, TEXT("Failed to join session."));
}

void UBSWServerBrowser::OnHostComplete()
{
	UE_LOG(BoneSplit, Display, TEXT("Host game complete, Travelling to Hub."));
	UBSGameManagerSubsystem::Get(GetOwningPlayer())->InitializeHost();
}

void UBSWServerBrowser::OnSessionFindFailure(const TArray<FBlueprintSessionResult>& Sessions)
{
	SearchingImage->SetVisibility(ESlateVisibility::Hidden);
	UE_LOG(BoneSplit, Display, TEXT("Failed to find any session."));
}

void UBSWServerBrowser::OnSessionsFound(const TArray<FBlueprintSessionResult>& Sessions)
{
	UE_LOG(BoneSplit, Display, TEXT("Session Search Complete"));
	
	SearchingImage->SetVisibility(ESlateVisibility::Hidden);
	for (auto& SessionResult : Sessions)
	{
		if (SessionResult.OnlineResult.IsValid())
		{
			FoundSessions.Add(SessionResult);
		}
	}
	
	for (int i = 0; i < FoundSessions.Num(); ++i)
	{
		UE_LOG(BoneSplit, Display, TEXT("%s"), 
			*FString("Found Session:" + FoundSessions[i].OnlineResult.Session.OwningUserName));
		
		UBSWServerButton* NewButton = CreateWidget<UBSWServerButton>(GetOwningPlayer(), ServerButtonClass);
		const FText SessionName = FText::FromString(Sessions[i].OnlineResult.Session.OwningUserName);
		const FText SessionPing = FText::FromString(FString::FromInt(Sessions[i].OnlineResult.PingInMs));
		const int32 MaxPlayers = Sessions[i].OnlineResult.Session.SessionSettings.NumPublicConnections;
		const int32 CurrentPlayers = Sessions[i].OnlineResult.Session.NumOpenPublicConnections;
		const FString MaxCount = FString::FromInt(MaxPlayers);
		const FString CurrentCount = FString::FromInt(MaxPlayers - CurrentPlayers);
		const FText SessionCount = FText::FromString(CurrentCount + "/" + MaxCount);
		
		NewButton->InitializeServerButton(SessionName, SessionCount, SessionPing);
		
		NewButton->OnClicked().AddWeakLambda(this, [this, i]()
		{
			JoinSessionAt(i);
		});
		
		SessionPanel->AddChild(NewButton);
	}
}
