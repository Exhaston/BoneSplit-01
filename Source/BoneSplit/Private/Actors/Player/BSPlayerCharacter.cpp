// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerCharacter.h"

#include "Actors/Player/BSPlayerMovementComponent.h"
#include "Actors/Player/BSPlayerState.h"
#include "Animation/BSAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/Inventory/BSEquipment.h"
#include "Components/Inventory/BSEquipmentMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/InteractionSystem/BSInteractionComponent.h"

#define CREATE_EQUIPMENT_MESH(ComponentName, DisplayName, ParentMesh, ParentBoneName, SetLeader) \
ComponentName = CreateDefaultSubobject<UBSEquipmentMeshComponent>(TEXT(DisplayName)); \
ComponentName->bUseBoundsFromLeaderPoseComponent = true; \
ComponentName->SetupAttachment(ParentMesh, ParentBoneName); \
ComponentName->SetLeaderPoseComponent(SetLeader ? ParentMesh : nullptr, true); \
ComponentName->SetReceivesDecals(false); \
ComponentName->SetCollisionProfileName("CharacterMesh");

ABSPlayerCharacter::ABSPlayerCharacter(const FObjectInitializer& ObjectInitializer) : 
Super(ObjectInitializer
	.SetDefaultSubobjectClass<UBSEquipmentMeshComponent>(MeshComponentName)
	.SetDefaultSubobjectClass<UBSPlayerMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
	SetNetUpdateFrequency(60);
	bAlwaysRelevant = true;
	SetReplicates(true);
	
	GetReplicatedMovement_Mutable().RotationQuantizationLevel = ERotatorQuantization::ShortComponents;
	
	// =================================================================================================================
	// Meshes
	// ================================================================================================================= 
	
	GetMesh()->SetReceivesDecals(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	GetMesh()->SetTickableWhenPaused(false);
	
	if (UBSEquipmentMeshComponent* EquipmentMesh = Cast<UBSEquipmentMeshComponent>(GetMesh()))
	{
		EquipmentMesh->MeshTag = BSTags::EquipmentMesh_Chest;
	}
	
	CREATE_EQUIPMENT_MESH(HeadComponent, "HeadMeshComponent", GetMesh(), NAME_None, true);
	HeadComponent->MeshTag = BSTags::EquipmentMesh_Head;
	CREATE_EQUIPMENT_MESH(ArmsComponent, "ArmsMeshComponent", GetMesh(), NAME_None, true);
	ArmsComponent->MeshTag = BSTags::EquipmentMesh_Arms;
	CREATE_EQUIPMENT_MESH(LegsComponent, "LegsMeshComponent", GetMesh(), NAME_None, true);
	LegsComponent->MeshTag = BSTags::EquipmentMesh_Legs;
	
	CREATE_EQUIPMENT_MESH(MainHandComponent, "MainHandComponent", GetMesh(), "MainHand", false);
	MainHandComponent->MeshTag = BSTags::EquipmentMesh_MainHand;
	CREATE_EQUIPMENT_MESH(OffHandComponent, "OffHandComponent", GetMesh(), "OffHand", false);
	OffHandComponent->MeshTag = BSTags::EquipmentMesh_Offhand;
	
	// =================================================================================================================
	// Camera
	// ================================================================================================================= 
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->SetRelativeLocation(FVector(0,0,100));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SocketOffset = FVector(0, 100, 25);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	bUseControllerRotationYaw = false;
	
	//This component should use a world space billboard material, hence absolute rotation
	PlayerNameTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PlayerNameTextComponent"));
	PlayerNameTextComponent->SetupAttachment(GetMesh());
	PlayerNameTextComponent->SetUsingAbsoluteRotation(true);
	PlayerNameTextComponent->SetText(FText::FromString("PlayerName"));
	PlayerNameTextComponent->SetHorizontalAlignment(EHTA_Center);
	PlayerNameTextComponent->SetVerticalAlignment(EVRTA_TextCenter);
	
	InteractionComponent = CreateDefaultSubobject<UBSInteractionComponent>(TEXT("InteractionComponent"));
}

void ABSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABSPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	BSConsoleVariables::CVarShowPlayerHoverNames->OnChangedDelegate().RemoveAll(this);
	BSConsoleVariables::CVarBSCameraOffset->OnChangedDelegate().RemoveAll(this);
	BSConsoleVariables::CVarShowOwnHoverName->OnChangedDelegate().RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ABSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeCharacter(); //Init for server
}

void ABSPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSPlayerCharacter, bIsDead);
	DOREPLIFETIME(ABSPlayerCharacter, bIsInCombat);
}

void ABSPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitializeCharacter(); //Init for Client(s)
}

void ABSPlayerCharacter::SetMenuCamera()
{
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->SetRelativeRotation({0, -45, 0});
}

void ABSPlayerCharacter::ResetCamera()
{
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetRelativeRotation({0, 90, 0});
}

void ABSPlayerCharacter::InitializeCharacter()
{
	if (ABSPlayerState* PS = GetPlayerState<ABSPlayerState>())
	{
		SetupFloatingName(PS);
		
		AbilitySystemComponent = PS->GetBSAbilitySystem();
		check(AbilitySystemComponent.IsValid());
		GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		
		if (UBSAnimInstance* AnimInstance = Cast<UBSAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			AnimInstance->InitializeAbilitySystemComponent(GetAbilitySystemComponent());
		}

		SetupMeshes(AbilitySystemComponent.Get());
		
		GetCharacterMovement<UBSPlayerMovementComponent>()->InitializeAsc(AbilitySystemComponent.Get());
		
		//Update current skeletal colors, and also subscribe for future changes.
		OnPlayerColourChanged(PS->GetPlayerColor());
		PS->GetOnPlayerColorChanged().AddDynamic(this, &ABSPlayerCharacter::OnPlayerColourChanged);
		
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

void ABSPlayerCharacter::SetupFloatingName(APlayerState* PS)
{
	if (!PS) return;
	if (!IsRunningDedicatedServer())
	{
		PlayerNameTextComponent->SetText(FText::FromString(PS->GetPlayerName()));
		
		if (!IsLocallyControlled())
		{	
			PlayerNameTextComponent.Get()->SetHiddenInGame(!BSConsoleVariables::CVarShowPlayerHoverNames->GetBool());
			BSConsoleVariables::CVarShowPlayerHoverNames->OnChangedDelegate().AddWeakLambda(
			this, [this](IConsoleVariable* ConsoleVariable)
			{
				if (IsValid(this))
				{
					PlayerNameTextComponent.Get()->SetHiddenInGame(!ConsoleVariable->GetBool());
				}
			});
		}
		else
		{
			PlayerNameTextComponent.Get()->SetHiddenInGame(!BSConsoleVariables::CVarShowOwnHoverName->GetBool());
			BSConsoleVariables::CVarShowOwnHoverName->OnChangedDelegate().AddWeakLambda(
			this, [this](IConsoleVariable* ConsoleVariable)
			{
				if (IsValid(this))
				{
					PlayerNameTextComponent.Get()->SetHiddenInGame(!ConsoleVariable->GetBool());
				}
			});
		}
	}
}


void ABSPlayerCharacter::SetupMeshes(UAbilitySystemComponent* InAsc)
{
	//Catch currents
	TArray<FActiveGameplayEffectHandle> ActiveHandles = 
		InAsc->GetActiveEffects(
			FGameplayEffectQuery::MakeQuery_MatchNoEffectTags(FGameplayTagContainer()));
	
	for (const auto ActiveEffectHandle : ActiveHandles)
	{
		const FActiveGameplayEffect* GameplayEffect = 
			InAsc->GetActiveGameplayEffect(ActiveEffectHandle);
		
		if (!GameplayEffect->Spec.Def.IsA(UBSEquipmentEffect::StaticClass())) continue;
		LoadMeshesFromEquipmentEffect(Cast<UBSEquipmentEffect>(GameplayEffect->Spec.Def));
	}
	                               
	//Subscribe to any further changes
	InAsc->OnActiveGameplayEffectAddedDelegateToSelf.AddWeakLambda(
		this,[this]
		(UAbilitySystemComponent* Asc, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
		{
			if (!Spec.Def.IsA(UBSEquipmentEffect::StaticClass())) return;
			LoadMeshesFromEquipmentEffect(Cast<UBSEquipmentEffect>(Spec.Def));
		});
}

void ABSPlayerCharacter::LoadMeshesFromEquipmentEffect(const UBSEquipmentEffect* EffectMeshComp)
{
	//Lazy and inefficient, consider caching. Shouldn't have a real impact though.
	TArray<UBSEquipmentMeshComponent*> MeshComps;
	GetComponents<UBSEquipmentMeshComponent>(MeshComps);

	if (EffectMeshComp->SlotTag.MatchesTagExact(BSTags::Equipment_Arms))
	{
		MainHandComponent->SetSkeletalMesh(nullptr);
		OffHandComponent->SetSkeletalMesh(nullptr);
	}
	
	for (auto& CurrentMeshInfo : EffectMeshComp->MeshInfo)
	{
		if (UBSEquipmentMeshComponent* FoundComp = *MeshComps.FindByPredicate(
		[this, &CurrentMeshInfo](const UBSEquipmentMeshComponent* MeshComp)
		{
			return MeshComp->MeshTag.MatchesTagExact(CurrentMeshInfo.SlotTag);
		}))
		{
			FoundComp->LazyLoadSkeletalMesh(CurrentMeshInfo.MeshAsset);
		}
	}
}

void ABSPlayerCharacter::OnPlayerStateInitComplete()
{
	const ABSPlayerState* PS = GetPlayerState<ABSPlayerState>();
	

	

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

ABSPlayerState* ABSPlayerCharacter::GetBSPlayerState() const
{
	return GetPlayerState<ABSPlayerState>();
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
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent.Get()->CancelAbilitiesWithTag(BSTags::Ability_Player_Legs);
	}
	
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

void ABSPlayerCharacter::OnPlayerColourChanged(const FColor NewColor)
{
	TArray<UBSEquipmentMeshComponent*> MeshComps;
	GetComponents<UBSEquipmentMeshComponent>(MeshComps);
	for (const auto MeshComp : MeshComps)
	{
		MeshComp->SetColor(NewColor);
	}
}
