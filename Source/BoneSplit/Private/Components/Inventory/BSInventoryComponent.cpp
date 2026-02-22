// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Inventory/BSInventoryComponent.h"

#include "AbilitySystemComponent.h"
#include "Actors/InteractableBases/BSEquipmentDropBase.h"
#include "BoneSplit/BoneSplit.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "GameSystems/BSGameManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/BSLocalWidgetSubsystem.h"


UBSInventoryComponent::UBSInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UBSInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBSInventoryComponent, CurrentColor);
}

void UBSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ValidateExistingWorldInstances();
	
	CacheInitialEquipment();
}

void UBSInventoryComponent::SetAbilitySystem(UAbilitySystemComponent* InAbilitySystemComponent)
{
	AbilitySystemComponent = InAbilitySystemComponent;
	
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddWeakLambda(this, [this]
		(UAbilitySystemComponent* Asc, const FGameplayEffectSpec& NewEffectSpec, FActiveGameplayEffectHandle NewHandle)
	{
		if (!NewEffectSpec.Def.IsA(UBSEquipmentEffect::StaticClass())) return;
		
		const UBSEquipmentEffect* NewActiveDef = Cast<UBSEquipmentEffect>(NewEffectSpec.Def);
		if (!NewActiveDef) return;
		
		// Query all active equipment effects
		const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchNoEffectTags(FGameplayTagContainer());

		for (TArray<FActiveGameplayEffectHandle> ActiveHandles = Asc->GetActiveEffects(Query); 
			const FActiveGameplayEffectHandle& Handle : ActiveHandles)
		{
			// Skip the one we just applied
			if (Handle == NewHandle) continue;
			
			const FActiveGameplayEffect* ActiveGE = Asc->GetActiveGameplayEffect(Handle);
			
			if (!ActiveGE->Spec.Def->IsA(UBSEquipmentEffect::StaticClass())) continue;

			if (const UBSEquipmentEffect* ExistingDef = Cast<UBSEquipmentEffect>(ActiveGE->Spec.Def); 
				ExistingDef->SlotTag.MatchesTagExact(NewActiveDef->SlotTag))
			{
				
				const FString LogInfoOccupied = "Equipment slot occupied, Removed old Equipment in favor of new: " 
				+ NewActiveDef->SlotTag.ToString();
				
				UE_LOG(BoneSplit, Log, TEXT("%s"), *LogInfoOccupied)
				Asc->RemoveActiveGameplayEffect(Handle);
			}
		}
		
		SendEquipmentCallback(NewActiveDef);
	});
}

void UBSInventoryComponent::Server_EquipItem_Implementation(const FBSEquipmentDropInfo& InDropInfo, const bool bDropOld)
{
	if (!InDropInfo.EquipmentEffect)
	{
		const FString LogInfoFailed = 
			"Equipment with GUID: " + InDropInfo.LootGuid.ToString() + " has invalid class. Failed to equip";
		
		UE_LOG(BoneSplit, Log, TEXT("%s"), *LogInfoFailed)
		return;
	}
	
	//Ensure the server has a valid guid matching the requested Equipment drop
	if (const int32 Index = EquipmentDropInfo.IndexOfByKey(InDropInfo); EquipmentDropInfo.IsValidIndex(Index))
	{
		//The data we use for application is that stored on the server. As it could be modifed on the client
		const FBSEquipmentDropInfo AuthorityEquipmentDropInfo = EquipmentDropInfo[Index];
		const FString LogInfoSuccess = "Requested Loot Valid: " 
		+ InDropInfo.EquipmentEffect->GetName() 
		+ "With GUID: " 
		+ InDropInfo.LootGuid.ToString();
		
		UE_LOG(BoneSplit, Log, TEXT("%s"), *LogInfoSuccess)
		
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			AuthorityEquipmentDropInfo.EquipmentEffect, 
			AuthorityEquipmentDropInfo.CurrentLevel,
			AbilitySystemComponent->MakeEffectContext());
		
		const UBSEquipmentEffect* EffectCDO = 
			GetDefault<UBSEquipmentEffect>(AuthorityEquipmentDropInfo.EquipmentEffect);
		
		TArray<FActiveGameplayEffectHandle> ActiveHandles = AbilitySystemComponent->GetActiveEffects(
					FGameplayEffectQuery::MakeQuery_MatchNoEffectTags(FGameplayTagContainer()));
			
		for (const auto& ActiveEffectHandle : ActiveHandles)
		{
			const FActiveGameplayEffect* ActiveGE = 
				AbilitySystemComponent->GetActiveGameplayEffect(ActiveEffectHandle);
			
			const UBSEquipmentEffect* OldExistingEffect = Cast<UBSEquipmentEffect>(ActiveGE->Spec.Def);
			
			//Remember we are iterating on EVERY effect right now, not only equipment effects. 
			//Might want to optimize by caching tags.
			if (!OldExistingEffect) continue;
			
			//If an old effect matches the one of a new, drop the old equipment on the new drops' location.
			//The reason why this happens here is we assume the new drop is deleted internally upon interaction. 
			//Even when identical effects are applied. Duplicate effects should never happen, 
			//as equipment effects should be set to stacking, and limited to 1.
			if (OldExistingEffect->SlotTag.MatchesTagExact(EffectCDO->SlotTag) && bDropOld)
			{			
				const FString LogInfoSpawnOld = "Equipment replaced, attempt spawn old equipment for: " 
				+ EffectCDO->SlotTag.ToString();
				
				UE_LOG(BoneSplit, Log, TEXT("%s"), *LogInfoSpawnOld)
				
				SpawnEquipmentForPlayer(OldExistingEffect->GetClass(), InDropInfo.EquipmentTransform);
			}
		}
		
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		
		//Remove this loot from valid loot, it was claimed successfully.
		EquipmentDropInfo.Remove(InDropInfo);
	}
	else
	{
		const FString LogInfoFailed = "Requested Loot Invalid: " 
		+ InDropInfo.EquipmentEffect->GetName() 
		+ "With GUID: " 
		+ InDropInfo.LootGuid.ToString() 
		+ ". Server had no record of this loot being active. Maybe it was already claimed, "
		"or spawned outside of the Equipment Component?";
		
		UE_LOG(BoneSplit, Log, TEXT("%s"), *LogInfoFailed)
	}
}

void UBSInventoryComponent::SpawnEquipmentForPlayer(
	const TSubclassOf<UBSEquipmentEffect> EquipmentEffect, const FTransform SpawnTransform, const int32 InLevel)
{
	if (!OwnerHasAuthority()) return;
	
	FBSEquipmentDropInfo SpawnInfo;
	SpawnInfo.LootGuid = FGuid::NewGuid();
	SpawnInfo.EquipmentEffect = EquipmentEffect;
	SpawnInfo.EquipmentTransform = SpawnTransform;
	SpawnInfo.CurrentLevel = InLevel;
	EquipmentDropInfo.Add(SpawnInfo);
	
	Client_SpawnEquipmentForPlayer(SpawnInfo);
}

TArray<const UBSEquipmentEffect*> UBSInventoryComponent::GetEquipment()
{
	return CachedCurrentEquipment;
}

void UBSInventoryComponent::CacheInitialEquipment()
{
	TArray<const UBSEquipmentEffect*> Equipment;
	
	// Query all active equipment effects
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchNoEffectTags(FGameplayTagContainer());

	for (TArray<FActiveGameplayEffectHandle> ActiveHandles = AbilitySystemComponent.Get()->GetActiveEffects(Query); 
		const FActiveGameplayEffectHandle& Handle : ActiveHandles)
	{
		const FActiveGameplayEffect* ActiveGE = AbilitySystemComponent.Get()->GetActiveGameplayEffect(Handle);
		if (!ActiveGE || !ActiveGE->Spec.Def) continue;

		const UBSEquipmentEffect* ExistingDef = Cast<UBSEquipmentEffect>(ActiveGE->Spec.Def);
		if (!ExistingDef) continue;

		Equipment.AddUnique(ExistingDef);
	}
	
	CachedCurrentEquipment = Equipment;
}

void UBSInventoryComponent::SendEquipmentCallback(const UBSEquipmentEffect* InNewEquipment)
{
	if (const UBSEquipmentEffect** FoundEffectPtr = CachedCurrentEquipment.FindByPredicate(
		[InNewEquipment](const UBSEquipmentEffect* EquipmentPtr)
		{
			return InNewEquipment->SlotTag == EquipmentPtr->SlotTag;
		}))
	{
		const UBSEquipmentEffect* FoundEffect = *FoundEffectPtr;
		OnPlayerEquipmentRemoved.Broadcast(FoundEffect);
		CachedCurrentEquipment.Remove(FoundEffect);
	}

	OnPlayerEquipmentEquipped.Broadcast(InNewEquipment);
	CachedCurrentEquipment.Add(InNewEquipment);
}

FColor UBSInventoryComponent::GetPlayerColor()
{
	if (const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>(); 
		DeveloperSettings->PlayerColors.IsValidIndex(CurrentColor))
	{
		return DeveloperSettings->PlayerColors[CurrentColor];
	}
	return FColor::White;
}

void UBSInventoryComponent::Server_SetPlayerColor_Implementation(const int32 NewColor)
{
	const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
	CurrentColor = NewColor;
	CurrentColor = FMath::Clamp(CurrentColor, 0, DeveloperSettings->PlayerColors.Num() - 1);
	OnRep_PlayerColor(); //For callbacks
}                 

void UBSInventoryComponent::ValidateExistingWorldInstances()
{
	//Add existing instances of loot placed in the world. 
	//TODO: Might have to do a transient check here. 
	//In theory since it's RPC based loot won't be added before the client is active, so it should be fine.
	if (GetOwner() && OwnerHasAuthority() && GetOwnerWorld()) 
	{
		if (!AbilitySystemComponent) return;
		
		TArray<AActor*> FoundEquipmentInWorld;
		UGameplayStatics::GetAllActorsOfClass(
			GetOwner(), 
			ABSEquipmentDropBase::StaticClass(),
			FoundEquipmentInWorld);

		for (const auto EquipmentInWorld : FoundEquipmentInWorld)
		{
			const ABSEquipmentDropBase* EquipmentDropBase = 
				Cast<ABSEquipmentDropBase>(EquipmentInWorld);
			
			if (EquipmentDropBase->LootSpawnInfo.EquipmentEffect && EquipmentDropBase->LootSpawnInfo.LootGuid.IsValid())
			{
				FBSEquipmentDropInfo SpawnInfo = EquipmentDropBase->LootSpawnInfo;
				SpawnInfo.EquipmentTransform = EquipmentDropBase->GetActorTransform();
				EquipmentDropInfo.Add(SpawnInfo);
			}
		}

		const FString LogSuccess = "Inventory Component Ready, registered " 
		+ FString::FromInt(FoundEquipmentInWorld.Num()) 
		+ " existing world items for " + GetOwner()->GetName();
		
		UE_LOG(BoneSplit, Log, TEXT("%s"), *LogSuccess)
	}
}

void UBSInventoryComponent::Client_SpawnEquipmentForPlayer_Implementation(const FBSEquipmentDropInfo& InDropInfo)
{
	ABSEquipmentDropBase* DropBaseActor = GetOwnerWorld()->SpawnActorDeferred<ABSEquipmentDropBase>(
		EquipmentDropClass, 
		InDropInfo.EquipmentTransform, 
		GetOwner(), 
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	DropBaseActor->InitializeLoot(InDropInfo);
	DropBaseActor->FinishSpawning(InDropInfo.EquipmentTransform);
}

void UBSInventoryComponent::OnRep_PlayerColor()
{
	if (const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>(); 
		DeveloperSettings->PlayerColors.IsValidIndex(CurrentColor))
	{
		OnPlayerColorChangedDelegate.Broadcast(DeveloperSettings->PlayerColors[CurrentColor]);
	}
	else
	{
		OnPlayerColorChangedDelegate.Broadcast(FColor::White);
	}
}

bool UBSInventoryComponent::OwnerHasAuthority()
{
	return GetOwner()->HasAuthority();
}

UWorld* UBSInventoryComponent::GetOwnerWorld()
{
	return GetOwner()->GetWorld();
}
