// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerCharacter.h"
#include "Actors/Player/BSPlayerMovementComponent.h"
#include "Actors/Player/BSSaveGame.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Components/Inventory/BSEquipment.h"
#include "Components/Inventory/BSEquipmentMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameInstance/BSPersistantDataSubsystem.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "GameState/BSTravelManager.h"
#include "Net/UnrealNetwork.h"

#define CREATE_EQUIPMENT_MESH(ComponentName, DisplayName, ParentMesh) \
ComponentName = CreateDefaultSubobject<UBSEquipmentMeshComponent>(TEXT(DisplayName)); \
ComponentName->bUseBoundsFromLeaderPoseComponent = true; \
ComponentName->SetupAttachment(ParentMesh); \
ComponentName->SetLeaderPoseComponent(ParentMesh); \
ComponentName->SetReceivesDecals(false); \
ComponentName->SetCollisionProfileName("CharacterMesh");

ABSPlayerCharacter::ABSPlayerCharacter(const FObjectInitializer& ObjectInitializer) : 
Super(ObjectInitializer
	.SetDefaultSubobjectClass<UBSEquipmentMeshComponent>(MeshComponentName)
	.SetDefaultSubobjectClass<UBSPlayerMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
	SetNetUpdateFrequency(30);
	bAlwaysRelevant = true;
	SetReplicates(true);
	
	GetMesh()->SetReceivesDecals(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	CREATE_EQUIPMENT_MESH(HeadComponent, "HeadMeshComponent", GetMesh());
	CREATE_EQUIPMENT_MESH(ArmsComponent, "ArmsMeshComponent", GetMesh());
	CREATE_EQUIPMENT_MESH(LegsComponent, "LegsMeshComponent", GetMesh());
}

void ABSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeCharacter(); //Init for server
}

void ABSPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitializeCharacter(); //Init for Client(s)
}

void ABSPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSPlayerCharacter, Equipment);
	DOREPLIFETIME(ABSPlayerCharacter, PlayerColor);
}

void ABSPlayerCharacter::InitializeCharacter()
{
	
	//TEST:::
	const UBSCalendarEventSubsystem* TimeDateSubsystem = UBSCalendarEventSubsystem::Get(this);
	check(TimeDateSubsystem);

	for (auto EventTag : TimeDateSubsystem->GetCurrentEventTags())
	{
		GEngine->AddOnScreenDebugMessage(
		-1,
		10,
		FColor::Green,
		FString::Printf(TEXT("%s"), *EventTag.GetTagName().ToString())
		);
	}
	
	UBSPersistantDataSubsystem* PersistantSubsystem = UBSPersistantDataSubsystem::Get(this);
	check(PersistantSubsystem);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	// =================================================================================================================
	// Authority Grants Attributes, Abilities, Tags Initial Equipment and Effects
	// ================================================================================================================= 
	
	if (HasAuthority())
	{
		
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
			AbilitySystemComponent->AddLooseGameplayTag(NewTag, 1, EGameplayTagReplicationState::TagAndCountToAll);
		}
		
		//Default items are applied regardless of save data containing more equipment. 
		//This is intentional as a fallback.
		
		for (const auto DefaultEquipment : SaveGameCDO->DefaultEquipment)
		{
			Server_ApplyEquipment(FBSEquipmentInstance(DefaultEquipment));
		}
	}
	
	// =================================================================================================================
	// Init SaveData
	// ================================================================================================================= 
	                
	if (IsLocallyControlled())
	{
		if (const APlayerController* PC = GetController<APlayerController>())
		{
			const UBSSaveGame* CurrentSave = PersistantSubsystem->GetOrLoadSaveGame(PC);
			Server_ReceiveClientSave(CurrentSave->SaveData);
		}
	}
}

void ABSPlayerCharacter::Server_ReceiveClientSave_Implementation(const FBSSaveData& SaveData)
{
	// =================================================================================================================
	// Server Handle Client Save Data
	// =================================================================================================================
	
	PlayerColor = SaveData.CurrentColor;
	
	for (auto ExistingAttribute : SaveData.Attributes)
	{
		//Override the base of an attribute to what was stored in the save.
		AbilitySystemComponent->SetNumericAttributeBase(ExistingAttribute.Attribute, ExistingAttribute.Value);
	}

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

	for (auto NewTag : SaveData.Tags)
	{
		AbilitySystemComponent->AddLooseGameplayTag(NewTag, 1, EGameplayTagReplicationState::TagAndCountToAll);
	}
	
	//Finally override default equipment with the saved equipment.
	for (const auto SavedEquipment : SaveData.Equipment)
	{
		Server_ApplyEquipment(SavedEquipment);
	}
	
	//Notify Client that init is complete, so it can remove loading screen and be ready.
	Client_InitComplete();
	
	//Add to ready players. This is unused at the moment, but can serve as a lock to ensure everyone is ready for RPCs.
	UBSTravelManager::GetTravelManager(this)->ReadyPlayers.AddUnique(this);
}

void ABSPlayerCharacter::Client_InitComplete_Implementation()
{
	// =================================================================================================================
	// Add UI
	// ================================================================================================================= 
	
	//TODO add UI here.
	
	// =================================================================================================================
	// Remove Loading Screen
	// ================================================================================================================= 
	
	if (UBSPersistantDataSubsystem* PersistantSubsystem = UBSPersistantDataSubsystem::Get(this))
	{
		PersistantSubsystem->RemoveLoadingScreen();
	}
}

UAbilitySystemComponent* ABSPlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ABSPlayerCharacter::LaunchActor(const FVector Direction, const float Magnitude)
{
	 Client_Launch(Direction, Magnitude);
}

void ABSPlayerCharacter::Client_Launch_Implementation(const FVector NormalizedDirection, const float Magnitude)
{
	GetCharacterMovement()->StopMovementImmediately();
	LaunchCharacter(
	(NormalizedDirection + FVector::UpVector).GetSafeNormal() * Magnitude * 2, true, true);
}

int32 ABSPlayerCharacter::FindEquipmentIndexByTag(const FGameplayTag& Slot) const
{
	const int32 FoundIndex = Equipment.IndexOfByPredicate(
	[Slot](const FBSEquipmentInstance& EquipmentInstance)
	{
		return Slot.MatchesTagExact( EquipmentInstance.GetSourceItemCDO()->SlotTag);
	});
		
	return FoundIndex;
}

void ABSPlayerCharacter::UpdateSkeletalMeshes()
{
	TArray<UBSEquipmentMeshComponent*> MeshComps;
	GetOwner()->GetComponents<UBSEquipmentMeshComponent>(MeshComps);
	for (auto EquipmentInstance : Equipment)
	{
		if (const UBSEquipment* EquipmentCDO = EquipmentInstance.GetSourceItemCDO(); 
		EquipmentCDO && EquipmentCDO->HasSkeletalMesh()) //ensure this is a slot that has skeletal mesh, and valid
		{
			for (const auto MeshComp : MeshComps)
			{
				if (MeshComp->MeshTag == EquipmentCDO->SlotTag)
				{
					MeshComp->LazyLoadSkeletalMesh(EquipmentCDO->SkeletalMesh);
				}
			}
		}
	}
}

void ABSPlayerCharacter::Server_ApplyEquipment_Implementation(FBSEquipmentInstance EquipmentInstance)
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
		
		UpdateSkeletalMeshes();
	}
}

void ABSPlayerCharacter::OnRep_Equipment()
{
	UpdateSkeletalMeshes();
}

void ABSPlayerCharacter::Server_ApplyColor_Implementation(const int32& NewColor)
{
	const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
	PlayerColor = FMath::Clamp(NewColor, 0, DeveloperSettings->PlayerColors.Num() - 1);
	OnRep_PlayerColor(); //Server call this too.
}

void ABSPlayerCharacter::OnRep_PlayerColor() const
{
	TArray<UBSEquipmentMeshComponent*> MeshComps;
	GetOwner()->GetComponents<UBSEquipmentMeshComponent>(MeshComps);
	for (const auto MeshComp : MeshComps)
	{
		MeshComp->SetMeshColorIndex(PlayerColor);
	}
}

void ABSPlayerCharacter::Internal_RemoveEquipment(const int32 Index)
{
	if (!Equipment.IsValidIndex(Index)) return;

	// Make a copy so we can safely use it after removal
	const FBSEquipmentInstance OldItem = Equipment[Index];
	Equipment.RemoveAt(Index);

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

void ABSPlayerCharacter::Internal_ApplyEquipment(const FBSEquipmentInstance& ItemInstance)
{
	if (!ItemInstance.GetSourceItemCDO()) return;
	
	Equipment.Add(ItemInstance);

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
