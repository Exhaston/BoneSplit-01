// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Equipment/BSEquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "LightmapResRatioAdjust.h"
#include "Actors/Player/BSPlayerState.h"
#include "BoneSplit/BoneSplit.h"
#include "Equipment/BSEquipmentAsset.h"
#include "Equipment/BSEquipmentInstance.h"
#include "Equipment/BSEquipmentPickupActor.h"
#include "Equipment/BSEquipmentSkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


UBSEquipmentComponent::UBSEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

UBSEquipmentComponent* UBSEquipmentComponent::GetEquipmentComponent(const APlayerState* PS)
{
	return PS->GetComponentByClass<UBSEquipmentComponent>();
}

void UBSEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwner() && GetOwner()->HasAuthority()) 
	{
		TArray<AActor*> FoundEquipmentInWorld;
		UGameplayStatics::GetAllActorsOfClass(
			GetOwner(), 
			ABSEquipmentPickupActor::StaticClass(),
			FoundEquipmentInWorld);

		for (const auto EquipmentInWorld : FoundEquipmentInWorld)
		{
			const ABSEquipmentPickupActor* EquipmentDropBase = 
				Cast<ABSEquipmentPickupActor>(EquipmentInWorld);
			
			if (EquipmentDropBase && EquipmentDropBase->ContainedEquipment.EquipmentInstance.HasValidData())
			{
				FBSEquipPickupInfo SpawnInfo = EquipmentDropBase->ContainedEquipment;
				SpawnInfo.WorldLocation = EquipmentDropBase->GetActorLocation();
				ServerWorldDropInfos.Add(SpawnInfo);
			}
		}

		const FString LogSuccess = "Inventory Component Ready, registered " 
		+ FString::FromInt(FoundEquipmentInWorld.Num()) 
		+ " existing world items for " + GetOwner()->GetName();
		
		UE_LOG(BoneSplit, Log, TEXT("%s"), *LogSuccess)
	}
}

void UBSEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBSEquipmentComponent, EquipmentInstances);
}

void UBSEquipmentComponent::Server_RequestEquipFromDrop_Implementation(const FBSEquipPickupInfo& Info)
{
	if (const int32 FoundIndex = ServerWorldDropInfos.Find(Info); FoundIndex != INDEX_NONE)
	{
		//Always use the server stored pickup data.
		const FBSEquipPickupInfo& ServerPickupInfo = ServerWorldDropInfos[FoundIndex];
		
		const float DistanceSq = FVector::DistSquared(
			OwnerCharacter->GetActorLocation(), ServerPickupInfo.WorldLocation);

		if (DistanceSq > FMath::Square(EquipmentServerMaxLeash))
		{
			UE_LOG(BoneSplit, Warning, TEXT("Server_ApplyEquipment: %s attempted out-of-range pickup"),
				*OwnerCharacter->GetName());
			return;
		}
		
		FBSEquipmentInstance OldEquipmentInstance;
		ApplyEquipment(ServerPickupInfo.EquipmentInstance, OldEquipmentInstance);
		
		if (OldEquipmentInstance.HasValidData())
		{
			FBSEquipPickupInfo NewEquipmentPickupInfo;
			NewEquipmentPickupInfo.EquipmentPickupInfo = FGuid::NewGuid();
			NewEquipmentPickupInfo.EquipmentInstance = OldEquipmentInstance;
			NewEquipmentPickupInfo.WorldLocation = ServerWorldDropInfos[FoundIndex].WorldLocation;
			
			DropEquipmentForPlayer(NewEquipmentPickupInfo);
		}
		
		ServerWorldDropInfos.RemoveAt(FoundIndex); //Invalidate the Equipment as claimed
	}
}

void UBSEquipmentComponent::DropEquipmentForPlayer(const FBSEquipPickupInfo& DropInfo)
{
	if (!GetOwner()->HasAuthority()) return;
	ServerWorldDropInfos.Add(DropInfo);
	Client_SpawnEquipment(DropInfo);
}

void UBSEquipmentComponent::SetEquipmentOwner(ACharacter* InOwnerCharacter, UAbilitySystemComponent* InOwnerAbilitySystem)
{
	check(InOwnerCharacter && InOwnerAbilitySystem);
	OwnerCharacter = InOwnerCharacter;
	OwnerAsc = InOwnerAbilitySystem;
	
	for (const FBSEquipmentInstance& Instance : EquipmentInstances)
	{
		if (!MeshInstances.Contains(Instance))
		{
			Internal_ApplyEquipmentInstance(Instance);
		}
	}
}

void UBSEquipmentComponent::ApplyEquipment(
	FBSEquipmentInstance NewEquipmentInstance,
	FBSEquipmentInstance& OldInstance)
{
	if (!IsOwnerAuthority()) return;
	if (!ensure(NewEquipmentInstance.HasValidData())) return;
	check(OwnerAsc && OwnerCharacter);
	
	if (FBSEquipmentInstance* ConflictingInstance = 
	EquipmentInstances.FindByPredicate([this, NewEquipmentInstance](const FBSEquipmentInstance& Current)
	{
		return Current.GetDef()->Slot == NewEquipmentInstance.GetDef()->Slot;
	}))
	{
		OldInstance = *ConflictingInstance;
		Internal_ClearEquipmentInstance(OldInstance);
		EquipmentInstances.Remove(OldInstance);
	}

	const FBSEquipmentInstance& NewInstance = EquipmentInstances.Add_GetRef(NewEquipmentInstance);
	Internal_ApplyEquipmentInstance(NewInstance);
}

void UBSEquipmentComponent::ApplyEquipmentByClass(const UBSEquipmentAsset* InEquipmentAsset)
{
	FBSEquipmentInstance OldInstance;
	ApplyEquipment(FBSEquipmentInstance(InEquipmentAsset), OldInstance);
}

void UBSEquipmentComponent::ForceRemoveEquipment(const UBSEquipmentAsset* InEquipmentAsset)
{
	if (FBSEquipmentInstance* ConflictingInstance = 
	EquipmentInstances.FindByPredicate([this, InEquipmentAsset](const FBSEquipmentInstance& Current)
	{
		return Current.GetDef() == InEquipmentAsset;
	}))
	{
		ConflictingInstance->ClearEquipmentInstance(OwnerAsc);
		EquipmentInstances.Remove(*ConflictingInstance);
	}
}

void UBSEquipmentComponent::ForceRemoveEquipment(const FBSEquipmentInstance& InEquipmentAsset)
{
	if (FBSEquipmentInstance* EquipmentInstance = EquipmentInstances.FindByKey(InEquipmentAsset))
	{
		EquipmentInstance->ClearEquipmentInstance(OwnerAsc);
		EquipmentInstances.Remove(*EquipmentInstance);
	}
}

void UBSEquipmentComponent::SetColorOnMeshes(const FColor InEquipmentColor)
{
	for (auto& EquipmentInstance : EquipmentInstances)
	{
		EquipmentInstance.SetMeshColor(InEquipmentColor);
	}
}

void UBSEquipmentComponent::Client_SpawnEquipment_Implementation(const FBSEquipPickupInfo& DropInfo)
{
	FTransform SpawnTransform = FTransform::Identity;
	SpawnTransform.SetTranslation(DropInfo.WorldLocation);
	
	ABSEquipmentPickupActor* NewDrop = 
		GetWorld()->SpawnActorDeferred<ABSEquipmentPickupActor>(
			EquipDropActorClass, 
			SpawnTransform, 
			GetOwner(), 
			nullptr, 
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	NewDrop->ContainedEquipment = DropInfo;
	
	NewDrop->FinishSpawning(SpawnTransform);
	
}

void UBSEquipmentComponent::OnRep_EquipmentInstances(TArray<FBSEquipmentInstance> OldInstances)
{
	if (!OwnerCharacter || !OwnerAsc) return;
	for (FBSEquipmentInstance& Current : EquipmentInstances)
	{
		const bool bIsNew = !OldInstances.ContainsByPredicate(
			[&Current](const FBSEquipmentInstance& Old) { return Old == Current; });

		if (bIsNew)
		{
			OnEquipmentApplied.Broadcast(Current);
			Internal_ApplyEquipmentInstance(Current);
		}
	}
	
	for (FBSEquipmentInstance& Old : OldInstances)
	{
		const bool bWasRemoved = !EquipmentInstances.ContainsByPredicate(
			[&Old](const FBSEquipmentInstance& Current) { return Current == Old; });

		if (bWasRemoved)
		{
			OnEquipmentRemoved.Broadcast(Old);
			Internal_ClearEquipmentInstance(Old);
		}
	}
}

void UBSEquipmentComponent::Internal_ApplyEquipmentInstance(const FBSEquipmentInstance EquipmentInstance)
{
	check(OwnerCharacter && OwnerCharacter->GetMesh());
	
	const AActor* ParentActor = OwnerCharacter;
	check(ParentActor);
	
	FBSActiveEquipmentData NewMeshInstance;
	
	if (IsOwnerAuthority())
	{
		check(OwnerAsc);
		for (const auto GrantedAbility : EquipmentInstance.GetDef()->GrantedAbilities)
		{
			NewMeshInstance.GrantedSpecHandles.Add(OwnerAsc->GiveAbility(GrantedAbility));
		}
		
		check(OwnerAsc);
		for (const auto AppliedEffect : EquipmentInstance.GetDef()->AppliedEffects)
		{
			FGameplayEffectSpecHandle SpecHandle = 
				OwnerAsc->MakeOutgoingSpec(
					AppliedEffect, EquipmentInstance.GetEquipmentLevel(), OwnerAsc->MakeEffectContext());
			
			NewMeshInstance.GrantedEffectHandles.Add(OwnerAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data));
		}
	}
	
	for (const auto& MeshData : EquipmentInstance.GetDef()->MeshData)
	{
		UBSEquipmentSkeletalMeshComponent* NewMeshComp = NewObject<UBSEquipmentSkeletalMeshComponent>(
			OwnerCharacter->GetMesh(), 
			UBSEquipmentSkeletalMeshComponent::StaticClass(), 
			NAME_None, 
			RF_Transient);
		
		if (!NewMeshComp) return;
		
		NewMeshComp->SetReceivesDecals(false);
		
		if (MeshData.bFollowPose)
		{
			NewMeshComp->SetLeaderPoseComponent(OwnerCharacter->GetMesh());
		}
		else if (MeshData.AnimBP)
		{
			
		}
		
		NewMeshComp->RegisterComponent();
		
		NewMeshComp->LoadSkeletalMeshAsync(MeshData.SkeletalMesh);
		
		NewMeshComp->AttachToComponent(
			OwnerCharacter->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			MeshData.SocketAttachName
		);
		
		
		NewMeshInstance.MeshComps.Add(NewMeshComp);
	}
	
	MeshInstances.Add(EquipmentInstance, NewMeshInstance);
}

void UBSEquipmentComponent::Internal_ClearEquipmentInstance(const FBSEquipmentInstance EquipmentInstance)
{
	if (FBSActiveEquipmentData* ExistingInstance = MeshInstances.Find(EquipmentInstance))
	{
		if (IsOwnerAuthority())
		{
			for (const auto& GrantedEffectHandle : ExistingInstance->GrantedEffectHandles)
			{
				OwnerAsc->RemoveActiveGameplayEffect(GrantedEffectHandle, 1);
			}
            
			for (const auto& GrantedSpecHandle : ExistingInstance->GrantedSpecHandles)
			{
				OwnerAsc->ClearAbility(GrantedSpecHandle);
			}
		}
        
		for (const auto& MeshComp : ExistingInstance->MeshComps)
		{
			if (MeshComp)
			{
				MeshComp->DestroyComponent();
			}
		}

		MeshInstances.Remove(EquipmentInstance);
	}
}

