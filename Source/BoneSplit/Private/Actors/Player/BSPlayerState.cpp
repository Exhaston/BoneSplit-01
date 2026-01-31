// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerState.h"

#include "Actors/Player/BSSaveGame.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Components/AbilitySystem/EffectBases/BSGameplayEffect.h"
#include "Components/TalentSystem/BSTalentComponent.h"
#include "Components/TalentSystem/BSTalentEffect.h"
#include "GameInstance/BSPersistantDataSubsystem.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "Net/UnrealNetwork.h"

ABSPlayerState::ABSPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponent->SetIsReplicated(true);
	
	AttributeSetSubObject = CreateDefaultSubobject<UBSAttributeSet>(TEXT("AttributeSet"));
	AbilitySystemComponent->AddAttributeSetSubobject(AttributeSetSubObject.Get());
	
	TalentComponent = CreateDefaultSubobject<UBSTalentComponent>("TalentComponent");
	
	bAlwaysRelevant = true;
	bReplicates = true;
	SetNetUpdateFrequency(30);
}

void ABSPlayerState::BeginPlay()
{
	Super::BeginPlay();
	TalentComponent.Get()->InitializeTalentComponent(AbilitySystemComponent);
}

void ABSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSPlayerState, EquipmentInstances);
	DOREPLIFETIME(ABSPlayerState, PlayerColor);
	DOREPLIFETIME(ABSPlayerState, bInitialized);
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
			if (UBSPersistantDataSubsystem* PersistantSubsystem = UBSPersistantDataSubsystem::Get(this))
			{
				PersistantSubsystem->RemoveLoadingScreen();
			}
		}
	}
}

void ABSPlayerState::Server_ReceiveSaveData_Implementation(const FBSSaveData& SaveData)
{
	InitDefaultSaveData();
	
	//Save Data Init
	RestoreAttributesFromSave(SaveData);
	RestoreEffectsFromSave(SaveData);
	RestoreTagsFromSave(SaveData);
	RestoreEquipmentFromSave(SaveData);
	RestoreTalentsFromSave(SaveData);
	
	bInitialized = true;
	OnRep_Initialized();
}

void ABSPlayerState::InitDefaultSaveData()
{
	const UBSPersistantDataSubsystem* PersistantSubsystem = UBSPersistantDataSubsystem::Get(this);
	check(PersistantSubsystem);
	
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("DID IT"));
	const UBSSaveGame* SaveGameCDO = GetDefault<UBSSaveGame>(PersistantSubsystem->SaveGameClass);

	for (const auto DefaultAbility : SaveGameCDO->DefaultAbilities)
	{
		AbilitySystemComponent->GiveAbility(DefaultAbility);
	}

	for (const auto DefaultEffect : SaveGameCDO->DefaultEffects)
	{
		FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(
			DefaultEffect, 1, AbilitySystemComponent->MakeEffectContext());

		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
	}
		
	for (auto NewTag : SaveGameCDO->DefaultTags)
	{
		AbilitySystemComponent->AddLooseGameplayTag(
			NewTag.Key, NewTag.Value, EGameplayTagReplicationState::TagAndCountToAll);
	}

	for (const auto DefaultEquipment : SaveGameCDO->DefaultEquipment)
	{
		Server_Equip(FBSEquipmentInstance(DefaultEquipment));
	}
}

void ABSPlayerState::Server_Equip_Implementation(const FBSEquipmentInstance EquipmentInstance)
{
	if (ensure(EquipmentInstance.GetSourceItemCDO()))
	{
		//Unequip old items with the same slot.
		if (const int32 FoundItemIndex = 
			FindEquipmentIndexByTag(EquipmentInstance.GetSourceItemCDO()->SlotTag); FoundItemIndex != INDEX_NONE)
		{
			Internal_RemoveEquipment(FoundItemIndex);
		}
		
		Internal_ApplyEquipment(EquipmentInstance);
		
		NotifyEquipmentUpdated();
	}
}

void ABSPlayerState::Server_SetColor_Implementation(const int32& NewColor)
{
	PlayerColor = NewColor;
	OnRep_PlayerColor();
}

FBSEquipmentInstance ABSPlayerState::FindEquipmentByTag(const FGameplayTag& Slot) const
{
	const FBSEquipmentInstance* FoundIndex = EquipmentInstances.FindByPredicate(
	[Slot](const FBSEquipmentInstance& EquipmentInstance)
	{
		return Slot.MatchesTagExact( EquipmentInstance.GetSourceItemCDO()->SlotTag);
	});
		
	return *FoundIndex;
}

int32 ABSPlayerState::FindEquipmentIndexByTag(const FGameplayTag& Slot) const
{
	const int32 FoundIndex = EquipmentInstances.IndexOfByPredicate(
	[Slot](const FBSEquipmentInstance& EquipmentInstance)
	{
		return Slot.MatchesTagExact( EquipmentInstance.GetSourceItemCDO()->SlotTag);
	});
		
	return FoundIndex;
}

void ABSPlayerState::OnRep_EquipmentInstances()
{
	NotifyEquipmentUpdated();
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

void ABSPlayerState::NotifyEquipmentUpdated()
{
	if (OnEquipmentUpdate.IsBound())
	{
		OnEquipmentUpdate.Broadcast(EquipmentInstances);
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

TArray<FBSEquipmentInstance> ABSPlayerState::GetCurrentEquipment() const
{
	return EquipmentInstances;
}

FBSSaveData& ABSPlayerState::GetSaveGame() const
{
	return UBSPersistantDataSubsystem::Get(this)->GetOrLoadSaveGame(GetPlayerController())->SaveData;
}

void ABSPlayerState::RestoreEffectsFromSave(const FBSSaveData& SaveData)
{
	for (const auto ExistingEffect : SaveData.Effects)
	{
		//Restore Effect from class
		const FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			ExistingEffect.EffectClass,
			1,
			Context
		);

		if (!SpecHandle.IsValid()) continue;

		// Restore stack count
		SpecHandle.Data->SetStackCount(ExistingEffect.StackCount);

		// Timed effect
		if (ExistingEffect.RemainingTime != 0)
		{
			SpecHandle.Data->SetDuration(
				ExistingEffect.RemainingTime < 0 ? -1 : ExistingEffect.RemainingTime, true);
		}

		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ABSPlayerState::RestoreTagsFromSave(const FBSSaveData& SaveData)
{
	for (auto NewTag : SaveData.Tags)
	{
		AbilitySystemComponent->AddLooseGameplayTag(NewTag, 1, EGameplayTagReplicationState::TagAndCountToAll);
	}
}

void ABSPlayerState::RestoreEquipmentFromSave(const FBSSaveData& SaveData)
{
	Server_SetColor(SaveData.CurrentColor);
	
	//Override default equipment with the saved equipment.
	for (const auto SavedEquipment : SaveData.Equipment)
	{
		Server_Equip(SavedEquipment);
	}
}

void ABSPlayerState::RestoreAttributesFromSave(const FBSSaveData& SaveData)
{
	for (auto ExistingAttribute : SaveData.Attributes)
	{
		//Override the base of an attribute to what was stored in the save.
		AbilitySystemComponent->SetNumericAttributeBase(ExistingAttribute.Attribute, ExistingAttribute.Value);
	}
}

void ABSPlayerState::RestoreTalentsFromSave(const FBSSaveData& SaveData)
{
	TalentComponent.Get()->Server_GrantSavedTalents(SaveData.GrantedTalents);
}

#pragma region SaveGame

void ABSPlayerState::SaveGameplayEffects(FBSSaveData& SaveData, UBSSaveGame* SaveGame)
{
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchNoEffectTags(FGameplayTagContainer());
	
	for (TArray<FActiveGameplayEffectHandle> ActiveHandles = GetAbilitySystemComponent()->GetActiveEffects(Query);
		const FActiveGameplayEffectHandle& Handle : ActiveHandles)
	{
		
		const FActiveGameplayEffect* ActiveEffect = GetAbilitySystemComponent()->GetActiveGameplayEffect(Handle);
		
		if (!ActiveEffect) continue;
		
		if (ActiveEffect->Spec.Def->DurationPolicy == EGameplayEffectDurationType::Instant)
		{
			continue;
		}
		
		if (!ActiveEffect->Spec.Def->IsA(UBSGameplayEffect::StaticClass()))
		{
			continue;
		}
		
		if (const UBSGameplayEffect* DefaultObject = 
			GetDefault<UBSGameplayEffect>(ActiveEffect->Spec.Def->GetClass()))
		{
			if (DefaultObject->bClearBetweenMaps)
			{
				continue;
			}
		}
		
		if (!ActiveEffect) continue;

		FBSGameplayEffectData EffectData;
		EffectData.RemainingTime = -1; //Init as infinite
		EffectData.EffectClass = ActiveEffect->Spec.Def->GetClass();
		EffectData.StackCount = ActiveEffect->Spec.GetStackCount();

		// Get remaining duration (if it’s a timed effect)
		if (ActiveEffect->GetDuration() > 0)
		{
			EffectData.RemainingTime = 
				ActiveEffect->GetTimeRemaining(GetAbilitySystemComponent()->GetWorld()->GetTimeSeconds());
		}

		SaveData.Effects.Add(EffectData);
	}
}

void ABSPlayerState::SaveGameplayTags(FBSSaveData& SaveData, UBSSaveGame* SaveGame)
{
	FGameplayTagContainer TagsToStore = AbilitySystemComponent->GetOwnedGameplayTags();
	
	const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
	
	TagsToStore = TagsToStore.Filter(DeveloperSettings->SavedGameplayTags);

	for (auto GameplayTag : TagsToStore)
	{                                                             
		for (int i = 0; i < AbilitySystemComponent->GetGameplayTagCount(GameplayTag) - 1; ++i)
		{
			SaveData.Tags.Add(GameplayTag);
		}
	}

	//Remove the default tags applied from the default save. Those will be applied next load regardless.
	for (auto DefaultTag : SaveGame->DefaultTags)
	{
		for (int i = 0; i < DefaultTag.Value - 1; ++i)
		{
			SaveData.Tags.RemoveSingle(DefaultTag.Key);
		}
	}
}

void ABSPlayerState::SaveEquipment(FBSSaveData& SaveData)
{
	SaveData.Equipment = EquipmentInstances;
	SaveData.CurrentColor = PlayerColor;
}

void ABSPlayerState::SaveAttributes(FBSSaveData& SaveData)
{
	TArray<FGameplayAttribute> CurrentAttributes;
	GetAbilitySystemComponent()->GetAllAttributes(CurrentAttributes);

	SaveData.Attributes.Empty();
	
	for (auto Attribute : CurrentAttributes)
	{
		FBSAttributeData AttributeData;
		AttributeData.Attribute = Attribute;
		//Only get the base. Current would cause conflicts with temporary effects.
		AttributeData.Value = GetAbilitySystemComponent()->GetNumericAttributeBase(Attribute);
		SaveData.Attributes.Add(AttributeData);
	}
}

void ABSPlayerState::SaveTalents(FBSSaveData& SaveData) const
{
	TArray<FBSTalentSaveData> SavedTalentData;
	for (const auto TalentData : TalentComponent.Get()->GetTalentData())
	{
		FBSTalentSaveData NewTalentData;
		NewTalentData.TalentClass = TalentData.Key;
		NewTalentData.CurrentLevel = TalentData.Value;
		SavedTalentData.Add(NewTalentData);
	}
	SaveData.GrantedTalents = SavedTalentData;
}

void ABSPlayerState::SaveState_Implementation(const bool bSaveToDisk)
{
	const APlayerController* PC = GetPlayerController();
	if (!PC) return;
	UBSPersistantDataSubsystem* PersistantSubsystem = UBSPersistantDataSubsystem::Get(this);
	UBSSaveGame* SG = PersistantSubsystem->GetOrLoadSaveGame(PC);
	
	SaveGameplayTags(SG->SaveData, SG);
	
	SaveAttributes(SG->SaveData);
	
	SaveGameplayEffects(SG->SaveData, SG);
	
	if (bSaveToDisk)
	{
		PersistantSubsystem->SaveGameToDiskSync(PC);	
	}
}

#pragma endregion

void ABSPlayerState::Internal_ApplyEquipment(const FBSEquipmentInstance& ItemInstance)
{
	if (!ItemInstance.GetSourceItemCDO()) return;
	
	EquipmentInstances.Add(ItemInstance);

	UAbilitySystemComponent* OwnerAsc = GetAbilitySystemComponent();
	check(OwnerAsc);

	for (const auto NewAbility : ItemInstance.GetSourceItemCDO()->GrantedAbilities)
	{
		OwnerAsc->GiveAbility(NewAbility);
	}

	for (const auto NewEffect : ItemInstance.GetSourceItemCDO()->GrantedEffects)
	{
		OwnerAsc->BP_ApplyGameplayEffectToSelf(NewEffect,
		ItemInstance.GetItemLevel(),
		OwnerAsc->MakeEffectContext());
	}
	
	for (auto Tag : ItemInstance.GetSourceItemCDO()->GrantedTags)
	{
		OwnerAsc->AddLooseGameplayTag(Tag, 1, EGameplayTagReplicationState::CountToOwner);
	}
}

void ABSPlayerState::Internal_RemoveEquipment(const int32 Index)
{
	if (!EquipmentInstances.IsValidIndex(Index)) return;

	// Make a copy so we can safely use it after removal
	const FBSEquipmentInstance OldItem = EquipmentInstances[Index];
	EquipmentInstances.RemoveAt(Index);

	UAbilitySystemComponent* OwnerAsc = GetAbilitySystemComponent();
	check(OwnerAsc);

	for (const auto OldAbility : OldItem.GetSourceItemCDO()->GrantedAbilities)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = OwnerAsc->FindAbilitySpecFromClass(OldAbility))
		{
			OwnerAsc->ClearAbility(AbilitySpec->Handle);
		}
	}

	for (const auto OldEffect : OldItem.GetSourceItemCDO()->GrantedEffects)
	{
		OwnerAsc->RemoveActiveGameplayEffectBySourceEffect(OldEffect, OwnerAsc, 1);
	}

	for (auto Tag : OldItem.GetSourceItemCDO()->GrantedTags)
	{
		OwnerAsc->RemoveLooseGameplayTag(Tag, 1, EGameplayTagReplicationState::CountToOwner);
	}
}
