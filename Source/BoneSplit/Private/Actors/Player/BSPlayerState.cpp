// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerState.h"

#include "Actors/Player/BSLocalSaveSubsystem.h"
#include "Actors/Player/BSSaveGame.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Components/TalentSystem/BSTalentComponent.h"
#include "GameInstance/BSLoadingScreenSubsystem.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "GameState/BSGameState.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/BSLocalWidgetSubsystem.h"

ABSPlayerState::ABSPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	AbilitySystemComponent->SetIsReplicated(true);
	
	AbilitySystemComponent->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(BSTags::GameplayEvent_DamageDealt), 
	FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &ABSPlayerState::OnDamageOther));
	
	AttributeSetSubObject = CreateDefaultSubobject<UBSAttributeSet>(TEXT("AttributeSet"));
	AbilitySystemComponent->AddAttributeSetSubobject(AttributeSetSubObject.Get());
	
	TalentComponent = CreateDefaultSubobject<UBSTalentComponent>("TalentComponent");
	TalentComponent->SetAbilitySystemComponent(AbilitySystemComponent.Get());
	
	bAlwaysRelevant = true;
	bReplicates = true;
	SetNetUpdateFrequency(30);
}

void ABSPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetPlayerController() && GetPlayerController()->IsLocalController())
	{
		UBSLocalSaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<UBSLocalSaveSubsystem>();
		SaveSubsystem->GetOnAsyncLoadCompleteDelegate().AddDynamic(this, &ABSPlayerState::OnSaveLoaded);
		SaveSubsystem->LoadGameAsync(GetPlayerController());
			
		SaveSubsystem->GetOnAsyncSaveCompleteDelegate().AddDynamic(this, &ABSPlayerState::OnAutoSave);
	}
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ABSPlayerState::OnSaveLoaded(UBSSaveGame* SaveGame)
{
	Server_ReceiveSaveData(SaveGame->SaveData);
		
	SetAutoSaveTimer();
}

void ABSPlayerState::SetAutoSaveTimer()
{
	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().SetTimer(AutoSaveHandle, [this]()
	{
		if (!GetWorld()) return;
		if (!IsActorBeingDestroyed() && !IsPendingKillPending())
		{
			UBSLocalSaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<UBSLocalSaveSubsystem>();
			//Ensure save is saved properly before continuing wanting a new save later
			SaveSubsystem->SaveAscDataAsync(GetPlayerController(), GetAbilitySystemComponent());
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(AutoSaveHandle);
		}
	}, 30, false, 30);
}

void ABSPlayerState::OnAutoSave()
{
	SetAutoSaveTimer();
	UE_LOG(BoneSplit, Display, TEXT("Auto Save Completed"));
}

void ABSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSPlayerState, PlayerColor);
	DOREPLIFETIME(ABSPlayerState, bInitialized);
}

UBSAbilitySystemComponent* ABSPlayerState::GetBSAbilitySystem() const
{
	return AbilitySystemComponent.Get();
}

void ABSPlayerState::OnDamageOther(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (HasAuthority())
	{
		Client_SpawnDamageNumber(*Payload);
	}
}

void ABSPlayerState::Client_SpawnDamageNumber_Implementation(const FGameplayEventData Payload)
{
	if (const APlayerController* PC = GetPlayerController())
	{
		if (PC->GetLocalPlayer())
		{
			UBSLocalWidgetSubsystem* WidgetSubsystem = PC->GetLocalPlayer()->GetSubsystem<UBSLocalWidgetSubsystem>();
			WidgetSubsystem->SpawnDamageNumber(Payload);
		}
	}
}

void ABSPlayerState::OnPlayerPaused()
{
	Server_ReceiveWantPause();
}

void ABSPlayerState::OnPlayerResumed()
{
	Server_ReceiveWantResume();
}

void ABSPlayerState::Server_ReceiveWantResume_Implementation()
{
	if (IsRunningDedicatedServer()) return;
	if (ABSGameState* GS = GetWorld()->GetGameState<ABSGameState>())
	{
		GS->Server_ReleasePauseRequest(this);
	}
}

void ABSPlayerState::Server_ReceiveWantPause_Implementation()
{
	if (IsRunningDedicatedServer()) return;
	if (ABSGameState* GS = GetWorld()->GetGameState<ABSGameState>())
	{
		GS->Server_RequestPause(this);
	}
}

void ABSPlayerState::OnRep_Initialized() const
{
	if (bInitialized)
	{
		if (OnInitComplete.IsBound())
		{
			OnInitComplete.Broadcast();
		}
		
		//UI
		if (HasLocalNetOwner() && !IsRunningDedicatedServer())
		{
			UBSLocalWidgetSubsystem* WidgetSubsystem = 
				GetPlayerController()->GetLocalPlayer()->GetSubsystem<UBSLocalWidgetSubsystem>();
			
			
			WidgetSubsystem->CreatePlayerUI(GetPlayerController());
			
			WidgetSubsystem->GetOnPauseMenuDelegate().AddDynamic(this, &ABSPlayerState::OnPlayerPaused);
			WidgetSubsystem->GetOnResumeDelegate().AddDynamic(this, &ABSPlayerState::OnPlayerResumed);
			
			UBSLoadingScreenSubsystem* LoadingScreenSubsystem = 
				GetPlayerController()->GetLocalPlayer()->GetSubsystem<UBSLoadingScreenSubsystem>();
			
			LoadingScreenSubsystem->RemoveLoadingScreen();
		}
	}
}

void ABSPlayerState::Server_ReceiveSaveData_Implementation(const FBSSaveData& SaveData)
{
	UBSLocalSaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<UBSLocalSaveSubsystem>();
	check(SaveSubsystem);
	
	SaveSubsystem->ApplyDefaultData(GetAbilitySystemComponent());
	SaveSubsystem->PopulateAscFromSaveData(GetAbilitySystemComponent(), SaveData);
	
	bInitialized = true;
	OnRep_Initialized();
}

void ABSPlayerState::Server_SetColor_Implementation(const int32& NewColor)
{
	PlayerColor = NewColor;
	OnRep_PlayerColor();
}

FColor ABSPlayerState::GetPlayerColor() const
{
	const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
	if (DeveloperSettings->PlayerColors.IsEmpty()) return FColor::White;
	const int32 Index = FMath::Clamp(PlayerColor, 0, DeveloperSettings->PlayerColors.Num() - 1);
	return DeveloperSettings->PlayerColors[Index];
}

void ABSPlayerState::OnRep_PlayerColor() const
{
	if (OnPlayerColorChangedDelegate.IsBound())
	{
		OnPlayerColorChangedDelegate.Broadcast(GetPlayerColor());
	}
}

UAbilitySystemComponent* ABSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

UBSTalentComponent* ABSPlayerState::GetTalentComponent() const
{
	return TalentComponent.Get();
}

bool ABSPlayerState::GetIsInitialized() const
{
	return bInitialized;
}