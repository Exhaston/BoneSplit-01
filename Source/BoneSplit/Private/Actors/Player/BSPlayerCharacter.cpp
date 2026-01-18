// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerCharacter.h"

#include "Actors/Player/BSPlayerMovementComponent.h"
#include "Actors/Player/BSSaveGame.h"
#include "Camera/CameraComponent.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/AbilitySystem/EffectBases/BSGameplayEffect.h"
#include "Components/Inventory/BSEquipment.h"
#include "Components/Inventory/BSEquipmentMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
	
	GetReplicatedMovement_Mutable().RotationQuantizationLevel = ERotatorQuantization::ShortComponents;
	
	GetMesh()->SetReceivesDecals(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	if (UBSEquipmentMeshComponent* EquipmentMesh = Cast<UBSEquipmentMeshComponent>(GetMesh()))
	{
		EquipmentMesh->MeshTag = BSTags::Equipment_Part_Chest;
	}
	
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	CREATE_EQUIPMENT_MESH(HeadComponent, "HeadMeshComponent", GetMesh());
	HeadComponent->MeshTag = BSTags::Equipment_Part_Head;
	CREATE_EQUIPMENT_MESH(ArmsComponent, "ArmsMeshComponent", GetMesh());
	ArmsComponent->MeshTag = BSTags::Equipment_Part_Arms;
	CREATE_EQUIPMENT_MESH(LegsComponent, "LegsMeshComponent", GetMesh());
	LegsComponent->MeshTag = BSTags::Equipment_Part_Legs;                             
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	bUseControllerRotationYaw = true;
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

void ABSPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABSPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsLocallyControlled())
	{
		SaveState(true);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ABSPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSPlayerCharacter, Equipment);
	DOREPLIFETIME(ABSPlayerCharacter, PlayerColor);
}

void ABSPlayerCharacter::InitializeCharacter()
{
	UBSPersistantDataSubsystem* PersistantSubsystem = UBSPersistantDataSubsystem::Get(this);
	check(PersistantSubsystem);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	// =================================================================================================================
	// Authority Grants Attributes, Abilities, Tags Initial Equipment and Effects
	// ================================================================================================================= 
	
	if (HasAuthority())
	{
		InitializeDefaultData(PersistantSubsystem);
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

#pragma region SaveGame

void ABSPlayerCharacter::SaveGameplayEffects(FBSSaveData& SaveData, UBSSaveGame* SaveGame)
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

void ABSPlayerCharacter::SaveGameplayTags(FBSSaveData& SaveData, UBSSaveGame* SaveGame)
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

void ABSPlayerCharacter::SaveEquipment(FBSSaveData& SaveData)
{
	SaveData.Equipment = Equipment;
	SaveData.CurrentColor = PlayerColor;
}

void ABSPlayerCharacter::SaveAttributes(FBSSaveData& SaveData)
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

void ABSPlayerCharacter::SaveState_Implementation(const bool bSaveToDisk)
{
	const APlayerController* PC = GetController<APlayerController>();
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

#pragma region LoadSave

void ABSPlayerCharacter::InitializeDefaultData(UBSPersistantDataSubsystem* PersistantSubsystem)
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
		AbilitySystemComponent->AddLooseGameplayTag(
			NewTag.Key, NewTag.Value, EGameplayTagReplicationState::TagAndCountToAll);
	}
		
	//Default items are applied regardless of save data containing more equipment. 
	//This is intentional as a fallback.
		
	for (const auto DefaultEquipment : SaveGameCDO->DefaultEquipment)
	{
		Server_ApplyEquipment(FBSEquipmentInstance(DefaultEquipment));
	}
}

void ABSPlayerCharacter::Server_ReceiveClientSave_Implementation(const FBSSaveData& SaveData)
{
	// =================================================================================================================
	// Server Handle Client Save Data
	// =================================================================================================================
	
	RestoreAttributesFromSave(SaveData);
	RestoreEffectsFromSave(SaveData);
	RestoreTagsFromSave(SaveData);
	RestoreEquipmentFromSave(SaveData);
	
	//Notify Client that init is complete, so it can remove loading screen and be ready.
	Client_InitComplete();
	
	//Add to ready players. This is unused at the moment, but can serve as a lock to ensure everyone is ready for RPCs.
	UBSTravelManager::GetTravelManager(this)->ReadyPlayers.AddUnique(this);
}

void ABSPlayerCharacter::RestoreEffectsFromSave(const FBSSaveData& SaveData)
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

void ABSPlayerCharacter::RestoreTagsFromSave(const FBSSaveData& SaveData)
{
	for (auto NewTag : SaveData.Tags)
	{
		AbilitySystemComponent->AddLooseGameplayTag(NewTag, 1, EGameplayTagReplicationState::TagAndCountToAll);
	}
}

void ABSPlayerCharacter::RestoreEquipmentFromSave(const FBSSaveData& SaveData)
{
	PlayerColor = SaveData.CurrentColor;
	
	//Override default equipment with the saved equipment.
	for (const auto SavedEquipment : SaveData.Equipment)
	{
		Server_ApplyEquipment(SavedEquipment);
	}
}

void ABSPlayerCharacter::RestoreAttributesFromSave(const FBSSaveData& SaveData)
{
	for (auto ExistingAttribute : SaveData.Attributes)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, 
			ExistingAttribute.Attribute.AttributeName + ": " + FString::FromInt(ExistingAttribute.Value));
		//Override the base of an attribute to what was stored in the save.
		AbilitySystemComponent->SetNumericAttributeBase(ExistingAttribute.Attribute, ExistingAttribute.Value);
	}
}

void ABSPlayerCharacter::BP_SaveState()
{
	SaveState(true);
}

#pragma endregion

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

bool ABSPlayerCharacter::CanBeKilled() const
{
	return true;
}

void ABSPlayerCharacter::OnKilled(AActor* Killer, float Damage)
{
	if (HasAuthority() && CanBeKilled())
	{
		Multicast_OnKilled(Killer, Damage);
	}
}

void ABSPlayerCharacter::Multicast_OnKilled_Implementation(AActor* Killer, float Damage)
{
}

void ABSPlayerCharacter::LaunchActor(const FVector Direction, const float Magnitude)
{
	//Server cancel any leg ability (typically a movement ability)
	if (HasAuthority())
	{
		AbilitySystemComponent.Get()->CancelAbilitiesWithTag(BSTags::Ability_Player_Legs);
	}
	Client_Launch(Direction, Magnitude);
}

void ABSPlayerCharacter::SetMovementRotationMode(const uint8 NewMovementMode)
{
	
	bUseControllerRotationYaw = true;
	/*
	const bool ControlRotationMode = 
		NewMovementMode == static_cast<uint8>(EBSMovementRotationMode::Ebs_ControlRotation);
	
	GetCharacterMovement()->bUseControllerDesiredRotation = ControlRotationMode;
	//GetCharacterMovement()->bOrientRotationToMovement = !ControlRotationMode;

	GetCharacterMovement()->RotationRate.Yaw = 750;

	bAligningToController = ControlRotationMode;
	*/
}

void ABSPlayerCharacter::Client_Launch_Implementation(const FVector NormalizedDirection, const float Magnitude)
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	LaunchCharacter(
	NormalizedDirection * Magnitude, true, true);
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
	GetComponents<UBSEquipmentMeshComponent>(MeshComps);
	for (auto& EquipmentInstance : Equipment)
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
