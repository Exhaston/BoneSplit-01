// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerCharacter.h"

#include "Actors/Player/BSPlayerMovementComponent.h"
#include "Actors/Player/BSPlayerState.h"
#include "Animation/BSAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/Inventory/BSEquipment.h"
#include "Components/Inventory/BSEquipmentMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameInstance/BSPersistantDataSubsystem.h"

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
	
	// =================================================================================================================
	// Meshes
	// ================================================================================================================= 
	
	GetMesh()->SetReceivesDecals(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	
	if (UBSEquipmentMeshComponent* EquipmentMesh = Cast<UBSEquipmentMeshComponent>(GetMesh()))
	{
		EquipmentMesh->MeshTag = BSTags::Equipment_Part_Chest;
	}
	
	CREATE_EQUIPMENT_MESH(HeadComponent, "HeadMeshComponent", GetMesh());
	HeadComponent->MeshTag = BSTags::Equipment_Part_Head;
	CREATE_EQUIPMENT_MESH(ArmsComponent, "ArmsMeshComponent", GetMesh());
	ArmsComponent->MeshTag = BSTags::Equipment_Part_Arms;
	CREATE_EQUIPMENT_MESH(LegsComponent, "LegsMeshComponent", GetMesh());
	LegsComponent->MeshTag = BSTags::Equipment_Part_Legs;
	
	MainHandComponent = CreateDefaultSubobject<UBSEquipmentMeshComponent>(TEXT("MainHandComponent"));
	MainHandComponent->SetupAttachment(GetMesh(), "MainHand");
	MainHandComponent->SetReceivesDecals(false);
	MainHandComponent->SetCollisionProfileName("CharacterMesh");
	MainHandComponent->MeshTag = BSTags::Equipment_Part_Weapon_Main;
	
	OffHandComponent = CreateDefaultSubobject<UBSEquipmentMeshComponent>(TEXT("OffHandComponent"));
	OffHandComponent->SetupAttachment(GetMesh(), "OffHand");
	OffHandComponent->SetReceivesDecals(false);
	OffHandComponent->SetCollisionProfileName("CharacterMesh");
	//Not needed for now, such a specific case we just reference the comp directly.
	OffHandComponent->MeshTag = BSTags::Internal_Part_Weapon_Off; 
	
	// =================================================================================================================
	// Camera
	// ================================================================================================================= 
	
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

void ABSPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABSPlayerCharacter::OnPlayerEquipmentChanged(TArray<FBSEquipmentInstance>& EquipmentInstances)
{
	UpdateSkeletalMeshes(EquipmentInstances);
}

void ABSPlayerCharacter::InitializeCharacter()
{
	if (ABSPlayerState* PS = GetPlayerState<ABSPlayerState>())
	{
		AbilitySystemComponent = PS->GetAbilitySystemComponent();
		AbilitySystemComponent.Get()->InitAbilityActorInfo(PS, this);
		GetCharacterMovement<UBSPlayerMovementComponent>()->InitializeAsc(AbilitySystemComponent.Get());
		
		if (IsLocallyControlled()) //Save only exists on the local machine
		{                         
			PS->Server_ReceiveSaveData(PS->GetSaveGame());
		}
		
		UpdateSkeletalColors(PS->GetPlayerColor());
		PS->GetOnPlayerColorChanged().AddDynamic(this, &ABSPlayerCharacter::OnPlayerColourChanged);
		
		UpdateSkeletalMeshes(PS->GetCurrentEquipment());
		PS->GetEquipmentUpdatedDelegate().AddDynamic(this, &ABSPlayerCharacter::OnPlayerEquipmentChanged);
		
		if (PS->GetIsInitialized())
		{
			OnPlayerStateInitComplete();
		}
		else
		{
			PS->GetInitCompleteDelegate().AddDynamic(this, &ABSPlayerCharacter::OnPlayerStateInitComplete);
		}
	}
}

void ABSPlayerCharacter::OnPlayerStateInitComplete()
{
	if (IsLocallyControlled())
	{
		//Local world space UI elements here
	}
	
	if (UBSAnimInstance* AnimInstance = Cast<UBSAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->NativeOnInitialized();
	}
}

bool ABSPlayerCharacter::BP_IsInCombat_Implementation()
{
	return bIsInCombat;
}

FBSOnCombatChangedDelegate& ABSPlayerCharacter::GetOnCombatChangedDelegate()
{
	return OnCombatChangedDelegate;
}

UAbilitySystemComponent* ABSPlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.IsValid() ? AbilitySystemComponent.Get() : nullptr;
}

void ABSPlayerCharacter::Client_LaunchCharacter_Implementation(const FVector LaunchVelocity, const bool bXYOverride,
	const bool bZOverride)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		
		LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
	}
}

void ABSPlayerCharacter::Launch(const FVector LaunchMagnitude, const bool bAdditive)
{
	//AbilitySystemComponent.Get()->CancelAbilitiesWithTag(BSTags::Ability_Player_Legs);
	
	if (HasAuthority() || IsLocallyControlled())
	{
		if (HasAuthority())
		{
			Multicast_OnLaunched(LaunchMagnitude);
		}

		Client_LaunchCharacter(LaunchMagnitude, !bAdditive, !bAdditive);
	}
}

void ABSPlayerCharacter::Multicast_OnLaunched_Implementation(FVector LaunchMagnitude)
{
	Execute_BP_OnLaunched(this, LaunchMagnitude);
}

void ABSPlayerCharacter::BP_OnLaunched_Implementation(FVector LaunchVelocity)
{
}

void ABSPlayerCharacter::OnRep_Death()
{
}

FBSOnDeathDelegate& ABSPlayerCharacter::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

void ABSPlayerCharacter::Die(UAbilitySystemComponent* SourceAsc, float Damage)
{
	if (HasAuthority())
	{
		bIsDead = true;

		Multicast_OnDeath(SourceAsc, Damage);
	}
}

void ABSPlayerCharacter::Multicast_OnDeath_Implementation(UAbilitySystemComponent* SourceAsc, float Damage)
{
	Execute_BP_OnDeath(this, SourceAsc, Damage);
	GetOnDeathDelegate().Broadcast(SourceAsc, GetAbilitySystemComponent(), Damage);
}

void ABSPlayerCharacter::BP_OnDeath_Implementation(UAbilitySystemComponent* SourceAsc, float Damage)
{
}

void ABSPlayerCharacter::OnRep_CombatChanged()
{
}

bool ABSPlayerCharacter::IsInCombat()
{
	return bIsInCombat;
}

void ABSPlayerCharacter::BP_OnCombatChanged_Implementation(bool InCombat)
{
}

void ABSPlayerCharacter::UpdateSkeletalMeshes(TArray<FBSEquipmentInstance> CurrentEquipment) const
{
	TArray<UBSEquipmentMeshComponent*> MeshComps;
	GetComponents<UBSEquipmentMeshComponent>(MeshComps);
	for (auto& EquipmentInstance : CurrentEquipment)
	{
		if (const UBSEquipment* EquipmentCDO = EquipmentInstance.GetSourceItemCDO(); 
		EquipmentCDO && EquipmentCDO->HasSkeletalMesh()) //ensure this is a slot that has skeletal mesh, and valid
		{
			const bool bIsWeapon = EquipmentCDO->SlotTag.MatchesTagExact(BSTags::Equipment_Part_Weapon_Main);
			
			if (UBSEquipmentMeshComponent* MatchingMesh = *MeshComps.FindByPredicate(
				[&EquipmentCDO](const UBSEquipmentMeshComponent* EquipmentMeshComponent)
			{
				return EquipmentCDO->SlotTag.MatchesTagExact(EquipmentMeshComponent->MeshTag);
			}))
			{
				MatchingMesh->LazyLoadSkeletalMesh(EquipmentCDO->SkeletalMesh);
			}
			
			if (bIsWeapon && !EquipmentCDO->OffHandSkeletalMesh.IsNull())
			{
				OffHandComponent->LazyLoadSkeletalMesh(EquipmentCDO->OffHandSkeletalMesh);
			}
		}
	}
}

void ABSPlayerCharacter::UpdateSkeletalColors(FColor CurrentColor)
{
	TArray<UBSEquipmentMeshComponent*> MeshComps;
	GetComponents<UBSEquipmentMeshComponent>(MeshComps);
	for (const auto MeshComp : MeshComps)
	{
		MeshComp->SetColor(CurrentColor);
	}
}

void ABSPlayerCharacter::OnPlayerColourChanged(const FColor NewColor)
{
	UpdateSkeletalColors(NewColor);
}
