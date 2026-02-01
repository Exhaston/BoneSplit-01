// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerState.h"

#include "Actors/Player/BSLocalSaveSubsystem.h"
#include "Actors/Player/BSSaveGame.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Components/TalentSystem/BSTalentComponent.h"
#include "GameInstance/BSLoadingScreenSubsystem.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "Net/UnrealNetwork.h"

ABSPlayerState::ABSPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	AbilitySystemComponent->SetIsReplicated(true);
	
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

void ABSPlayerState::OnRep_Initialized() const
{
	if (bInitialized)
	{
		if (OnInitComplete.IsBound())
		{
			OnInitComplete.Broadcast();
		}
		
		if (HasLocalNetOwner())
		{
			UBSLoadingScreenSubsystem* PSDS = GetPlayerController()->GetLocalPlayer()->GetSubsystem<UBSLoadingScreenSubsystem>();
			PSDS->RemoveLoadingScreen();
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