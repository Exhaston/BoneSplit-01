// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerState.h"

#include "Actors/InteractableBases/BSEquipmentDropBase.h"
#include "Actors/Player/BSGameplayHud.h"
#include "Actors/Player/BSLocalSaveSubsystem.h"
#include "Actors/Player/BSSaveGame.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Components/Inventory/BSInventoryComponent.h"
#include "Components/TalentSystem/BSTalentComponent.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "GameState/BSGameState.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/BSLocalWidgetSubsystem.h"

ABSPlayerState::ABSPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetOwnerActor(this);
	
	AbilitySystemComponent->AddGameplayEventTagContainerDelegate(
		FGameplayTagContainer(BSTags::GameplayEvent_DamageDealt), 
		
	FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &ABSPlayerState::OnDamageOther));
	
	TalentComponent = CreateDefaultSubobject<UBSTalentComponent>("TalentComponent");
	TalentComponent->SetAbilitySystemComponent(AbilitySystemComponent.Get());
	
	InventoryComponent = CreateDefaultSubobject<UBSInventoryComponent>("InventoryComponent");
	InventoryComponent->SetAbilitySystem(AbilitySystemComponent.Get());
	
	bAlwaysRelevant = true;
	bReplicates = true;
	SetNetUpdateFrequency(60);
}

void ABSPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddSet<UBSAttributeSet>();
	}
}

void ABSPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		TArray<AActor*> EquipmentFound;
		UGameplayStatics::GetAllActorsOfClass(this, ABSEquipmentDropBase::StaticClass(), EquipmentFound);
		for (const auto FoundEquipment : EquipmentFound)
		{
			const ABSEquipmentDropBase* EquipmentDropBase = Cast<ABSEquipmentDropBase>(FoundEquipment);
			FBSEquipmentDropInfo NewLootInfo;
			NewLootInfo.LootGuid = EquipmentDropBase->LootSpawnInfo.LootGuid;
			NewLootInfo.EquipmentEffect = EquipmentDropBase->LootSpawnInfo.EquipmentEffect;
	
			LootSpawnInfo.AddUnique(NewLootInfo);
		}
	}
	
	if (GetIsInitialized())
	{
		UE_LOG(BoneSplit, Log, TEXT("Skipping save load - already initialized via travel"));
		return;
	}
	
	if (GetPlayerController() && GetPlayerController()->IsLocalController() && GetPlayerController()->IsPrimaryPlayer())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Loading save"));
		UBSLocalSaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<UBSLocalSaveSubsystem>();
		SaveSubsystem->GetOnAsyncLoadCompleteDelegate().AddDynamic(this, &ABSPlayerState::OnSaveLoaded);
		SaveSubsystem->LoadGameAsync(GetPlayerController());
	}
}

void ABSPlayerState::SeamlessTravelTo(APlayerState* NewPlayerState)
{
	Super::SeamlessTravelTo(NewPlayerState);
}

void ABSPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	ABSPlayerState* NewPS = Cast<ABSPlayerState>(PlayerState);
	if (!NewPS) return;
	
	UAbilitySystemComponent* NewAsc = NewPS->GetAbilitySystemComponent();
	UAbilitySystemComponent* OldAsc = GetAbilitySystemComponent();
	
	TArray<FGameplayAttribute> Attributes;
	GetAbilitySystemComponent()->GetAllAttributes(Attributes);
	for (auto& Attribute : Attributes)
	{
		NewAsc->SetNumericAttributeBase(Attribute, OldAsc->GetNumericAttributeBase(Attribute));
	}
	
	TArray<FGameplayTagContainer> SpecTags;
	
	TArray<FGameplayEffectSpec> SpecCopies;

	TArray<FActiveGameplayEffectHandle> ActiveGEHandles = OldAsc->GetActiveEffects(
		FGameplayEffectQuery::MakeQuery_MatchNoEffectTags({}));

	for (const auto& ActiveGEHandle : ActiveGEHandles)
	{
		if (!ActiveGEHandle.IsValid()) continue;
		
		const FActiveGameplayEffect* ActiveGE = OldAsc->GetActiveGameplayEffect(ActiveGEHandle);
		if (!ActiveGE) continue;
		
		const FGameplayEffectContextHandle Context = NewAsc->MakeEffectContext();

		FGameplayEffectSpecHandle SpecHandle = NewPS->GetAbilitySystemComponent()->MakeOutgoingSpec(
			ActiveGE->Spec.Def->GetClass(),
			1,
			Context
		);
		
		FGameplayTagContainer SpecCopyTags;
		ActiveGE->Spec.GetAllGrantedTags(SpecCopyTags);
		SpecTags.Add(SpecCopyTags);
		SpecHandle.Data->SetStackCount(ActiveGE->Spec.GetStackCount());
		SpecHandle.Data->SetDuration(ActiveGE->GetTimeRemaining(OldAsc->GetWorld()->GetTimeSeconds()), 
			true);
		NewPS->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
	
	TArray<FGameplayTag> ActualTags;

	for (auto& Tag : GetAbilitySystemComponent()->GetOwnedGameplayTags().GetGameplayTagArray())
	{
		for (int i = 0; i < GetAbilitySystemComponent()->GetTagCount(Tag); ++i)
		{
			ActualTags.Add(Tag);
		}
	}

	for (auto& SpecTagContainer : SpecTags)
	{
		for (auto SpecTag : SpecTagContainer.GetGameplayTagArray())
		{
			ActualTags.RemoveSingle(SpecTag);
		}
	}

	for (auto& ActualTag : ActualTags)
	{
		NewPS->GetAbilitySystemComponent()->AddLooseGameplayTag(ActualTag, 1, EGameplayTagReplicationState::TagAndCountToAll);
	}
	
	NewPS->bInitialized = true;
}

void ABSPlayerState::OnDeactivated()
{
	Super::OnDeactivated();
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ABSPlayerState::OnSaveLoaded(UBSSaveGame* SaveGame)
{
	Server_ReceiveSaveData(SaveGame->SaveData);
}

void ABSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSPlayerState, bInitialized);
}

UBSAbilitySystemComponent* ABSPlayerState::GetBSAbilitySystem() const
{
	return AbilitySystemComponent.Get();
}

void ABSPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
}

UBSInventoryComponent* ABSPlayerState::GetInventoryComponent()
{
	return InventoryComponent.Get();
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
	if (const APlayerController* PC = GetPlayerController(); PC && PC->GetLocalPlayer())
	{
		PC->GetHUD<ABSGameplayHud>()->SpawnDamageNumber(Payload);
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
		OnPlayerStateReadyDelegate.Broadcast();
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