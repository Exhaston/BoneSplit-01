// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerCharacter.h"

#include "Actors/Player/BSGameplayHud.h"
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
	GetMesh()->SetVisibility(false);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	
	BSConsoleVariables::CVarToggleGear->OnChangedDelegate().AddWeakLambda(
		this, [this](IConsoleVariable* ConsoleVariable)
	{
			if (ConsoleVariable->GetBool())
			{
				GetMesh()->SetVisibility(true, true);
				GetMesh()->SetVisibility(false, false);
			}
			else
			{
				GetMesh()->SetVisibility(false, true);
				GetMesh()->SetVisibility(true, false);
			}
	});
	
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
		
		if (PS->GetIsInitialized())
		{
			PostPlayerStateInitialize();
		}
		else
		{
			PS->OnPlayerStateReadyDelegate.AddWeakLambda(this, [this]()
			{
				PostPlayerStateInitialize();
			});
		}
	}
}

void ABSPlayerCharacter::PostPlayerStateInitialize()
{
	ABSPlayerState* PS = GetPlayerState<ABSPlayerState>();
	UBSInventoryComponent* InventoryComponent = PS->GetInventoryComponent();
	
	//Initialize Asc on the character movement comp for stat tracking
	GetCharacterMovement<UBSPlayerMovementComponent>()->InitializeAsc(GetAbilitySystemComponent());
	
	//Bind Anim Instance to the ability system component.
	if (UBSAnimInstance* AnimInstance = Cast<UBSAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->InitializeAbilitySystemComponent(GetAbilitySystemComponent());
	}
	
	//Update meshes to equipment then subscribe to future events
	for (const auto ExistingEquipment : PS->GetInventoryComponent()->GetEquipment())
	{
		AddEquipmentMesh(ExistingEquipment);
	}
	
	PS->GetInventoryComponent()->GetEquipmentRemovedDelegate().AddWeakLambda(this, [this]
		(const UBSEquipmentEffect* OldEquipment)
	{
		RemoveEquipmentMesh(OldEquipment);
	});
	
	PS->GetInventoryComponent()->GetEquipmentEquippedDelegate().AddWeakLambda(
		this, [this](const UBSEquipmentEffect* Equipment)
	{
			AddEquipmentMesh(Equipment);
	});
	
	//Update Colors and subscribe to future events
	UpdateMeshColors(InventoryComponent->GetPlayerColor());
	InventoryComponent->GetPlayerColorChangedDelegate().AddWeakLambda(this, [this](FColor NewColor)
	{
		UpdateMeshColors(NewColor);
	});          
	
	//Lastly, after everything is set up, spawn UI for this player locally.
	if (const APlayerController* PC = GetController<APlayerController>(); PC && PC->IsLocalController())
	{
		PC->GetHUD<ABSGameplayHud>()->SpawnWidgets();
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

UBSInventoryComponent* ABSPlayerCharacter::GetInventoryComponent()
{
	return GetBSPlayerState()->GetInventoryComponent();
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

void ABSPlayerCharacter::UpdateMeshColors(const FColor NewColor)
{
	TArray<UBSEquipmentMeshComponent*> MeshComps;
	GetComponents<UBSEquipmentMeshComponent>(MeshComps);
	for (const auto MeshComp : MeshComps)
	{
		MeshComp->SetColor(NewColor);
	}
}

void ABSPlayerCharacter::AddEquipmentMesh(const UBSEquipmentEffect* InSource)
{
	FBSEquipmentMeshInfo NewInfo;
	NewInfo.SourceEffect = InSource;
	for (auto EquipmentMesh : InSource->EquipmentMeshes)
	{
		UBSEquipmentMeshComponent* NewMeshComp = NewObject<UBSEquipmentMeshComponent>(this, UBSEquipmentMeshComponent::StaticClass());
		if (!NewMeshComp) return;
		
		NewMeshComp->OnSkeletalMeshSetDelegate.AddWeakLambda(this, [this, NewMeshComp](USkeletalMesh* SkeletalMesh)
		{
			if (NewMeshComp)
			{
				OnEquipmentMeshChanged.Broadcast(NewMeshComp, SkeletalMesh);
			}
		});
		
		NewMeshComp->LazyLoadSkeletalMesh(EquipmentMesh.SkeletalMesh);
		
		if (EquipmentMesh.bFollowPose)
		{
			NewMeshComp->SetLeaderPoseComponent(GetMesh());
		}
		else if (EquipmentMesh.AnimBP)
		{
			NewMeshComp->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
			NewMeshComp->SetAnimInstanceClass(EquipmentMesh.AnimBP);
		}
		
		NewMeshComp->SetReceivesDecals(false);
		NewMeshComp->SetCollisionProfileName("CharacterMesh");
		
		NewMeshComp->SetColor(GetInventoryComponent()->GetPlayerColor());
		
		NewMeshComp->RegisterComponent();
		
		NewMeshComp->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			EquipmentMesh.SocketAttachName
		);
		
		NewInfo.EquipmentMeshComponents.Add(NewMeshComp);
	}
	
	EquipmentMeshInfos.Add(NewInfo);
}

void ABSPlayerCharacter::RemoveEquipmentMesh(const UBSEquipmentEffect* InSource)
{
	for (auto It = EquipmentMeshInfos.CreateIterator(); It; ++It)
	{
		if (It->SourceEffect == InSource)
		{
			for (const auto EquipmentMeshComponent : It->EquipmentMeshComponents)
			{
				EquipmentMeshComponent->DestroyComponent();	
			}
			It.RemoveCurrent();
			break;
		}
	}
}
