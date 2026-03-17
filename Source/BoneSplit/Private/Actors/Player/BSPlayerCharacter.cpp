// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerCharacter.h"

#include "NavigationInvokerComponent.h"
#include "PawnInitializationComponent.h"
#include "Abilities/BSCharacterInitData.h"
#include "Actors/Player/BSPlayerController.h"
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
#include "CharacterAbilityBase.h"
#include "Abilities/BSGameplayLibrary.h"
#include "Components/InteractionSystem/BSInteractionComponent.h"
#include "Equipment/BSEquipmentComponent.h"

#define CREATE_EQUIPMENT_MESH(ComponentName, DisplayName, ParentMesh, ParentBoneName, SetLeader) \
ComponentName = CreateDefaultSubobject<UBSEquipmentMeshComponent>(TEXT(DisplayName)); \
ComponentName->bUseBoundsFromLeaderPoseComponent = true; \
ComponentName->SetupAttachment(ParentMesh, ParentBoneName); \
ComponentName->SetLeaderPoseComponent(SetLeader ? ParentMesh : nullptr, true); \
ComponentName->SetReceivesDecals(false); \
ComponentName->SetCollisionProfileName("CharacterMesh");

UBSEquipmentComponent* ABSPlayerCharacter::GetEquipmentComponent() const
{
	return GetBSPlayerState()->GetEquipmentComponent();
}

void ABSPlayerCharacter::ApplyEquipment(const FBSEquipPickupInfo& Pickup)
{
	GetEquipmentComponent()->Server_RequestEquipFromDrop(Pickup);
}

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
	
	NavigationInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvokerComponent"));
	
	InitializationComponent = CreateDefaultSubobject<UPawnInitializationComponent>(TEXT("PawnExtensionComponent"));
	InitializationComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	InitializationComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
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
	
	if (ABSPlayerState* PS = GetBSPlayerState())
	{
		InitializationComponent->TryInitAbilitySystemForPlayer(PS->GetCharacterAbilitySystem(), PS);
	}

	
	InitializationComponent->HandleControllerChanged();
}

void ABSPlayerCharacter::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);
	
	if (ABSPlayerState* PS = GetBSPlayerState())
	{
		InitializationComponent->TryInitAbilitySystemForPlayer(PS->GetCharacterAbilitySystem(), PS);
	}
}

void ABSPlayerCharacter::UnPossessed()
{
	Super::UnPossessed();
	
	InitializationComponent->HandleControllerChanged();
}

void ABSPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	
	if (ABSPlayerState* PS = GetBSPlayerState())
	{
		InitializationComponent->TryInitAbilitySystemForPlayer(PS->GetCharacterAbilitySystem(), PS);
	}
	
	InitializationComponent->HandleControllerChanged();
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
	
	if (ABSPlayerState* PS = GetBSPlayerState())
	{
		InitializationComponent->TryInitAbilitySystemForPlayer(PS->GetCharacterAbilitySystem(), PS);
	}
}

void ABSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (ABSPlayerState* PS = GetBSPlayerState())
	{
		InitializationComponent->TryInitAbilitySystemForPlayer(PS->GetCharacterAbilitySystem(), PS);
	}
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

void ABSPlayerCharacter::OnDamageBlocked_Implementation()
{
	if (BlockMontage)
	{
		PlayAnimMontage(BlockMontage, 1);
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
	return InitializationComponent->GetCharacterAbilitySystem();
}

UBSInventoryComponent* ABSPlayerCharacter::GetInventoryComponent()
{
	return GetBSPlayerState()->GetInventoryComponent();
}

void ABSPlayerCharacter::OnAbilitySystemInitialized()
{
	UCharacterAbilitySystem* CharAsc = GetCharacterAbilitySystemComponent();
	check(CharAsc);
	
	if (HasAuthority())
	{	
		
		if (CharacterInitializationData)
		{
			UBSGameplayLibrary::ApplyCharacterDataTo(
				CharacterInitializationData, 
				CharAsc, 
				CharacterInitializationData->StartLevel);
		}
		
		CharAsc->GenericGameplayEventCallbacks.FindOrAdd(BSTags::GameplayEvent_Block).AddWeakLambda(
		this,[this](const FGameplayEventData* Payload)
		{
			if (Payload)
			{
				OnDamageBlocked();
			}
		});
	}
	
	if (ABSPlayerState* PS = GetPlayerState<ABSPlayerState>())
	{
		SetupFloatingName(PS);
		
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
		
		GetEquipmentComponent()->SetEquipmentOwner(this, GetCharacterAbilitySystemComponent());
		
		if (HasAuthority())
		{
			for (const auto DefEquip : DefaultEquipment)
			{
				GetEquipmentComponent()->ApplyEquipmentByClass(DefEquip);
			}
		}
	}
	
	if (GetBSPlayerController()) //Only for server and locally controlled
	{
		GetBSPlayerController()->InitializeWithAbilitySystem(CharAsc);
	}
}

bool ABSPlayerCharacter::CanJumpInternal_Implementation() const
{
	bool bJumpIsAllowed = GetCharacterMovement()->CanAttemptJump();
    
	const bool bCoyoteActive = 
		GetCharacterMovement<UBSPlayerMovementComponent>()->IsCoyoteTimeActive();

	if (bJumpIsAllowed || bCoyoteActive)
	{
		if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
		{
			if (bCoyoteActive)
			{
				// Coyote jump always allowed regardless of jump count
				bJumpIsAllowed = true;
			}
			else if (JumpCurrentCount == 0 && GetCharacterMovement()->IsFalling())
			{
				bJumpIsAllowed = JumpCurrentCount + 1 < JumpMaxCount;
			}
			else
			{
				bJumpIsAllowed = JumpCurrentCount < JumpMaxCount;
			}
		}
	}

	return bJumpIsAllowed;
}

void ABSPlayerCharacter::OnAbilitySystemUninitialized()
{
	
}

ABSPlayerState* ABSPlayerCharacter::GetBSPlayerState() const
{
	return CastChecked<ABSPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

ABSPlayerController* ABSPlayerCharacter::GetBSPlayerController() const
{
	return CastChecked<ABSPlayerController>(GetController(), ECastCheckedType::NullAllowed);
}

UCharacterAbilitySystem* ABSPlayerCharacter::GetCharacterAbilitySystemComponent() const
{
	return Cast<UCharacterAbilitySystem>(GetAbilitySystemComponent());
}

void ABSPlayerCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UCharacterAbilitySystem* CharAsc = GetCharacterAbilitySystemComponent())
	{
		CharAsc->GetOwnedGameplayTags(TagContainer);
	}
}

bool ABSPlayerCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UCharacterAbilitySystem* CharAsc = GetCharacterAbilitySystemComponent())
	{
		return CharAsc->HasMatchingGameplayTag(TagToCheck);
	}  
	return false;
}

bool ABSPlayerCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UCharacterAbilitySystem* CharAsc = GetCharacterAbilitySystemComponent())
	{
		return CharAsc->HasAllMatchingGameplayTags(TagContainer);
	}  
	return false;
}

bool ABSPlayerCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UCharacterAbilitySystem* CharAsc = GetCharacterAbilitySystemComponent())
	{
		return CharAsc->HasAnyMatchingGameplayTags(TagContainer);
	}  
	return false;
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
	if (GetCharacterAbilitySystemComponent())
	{
		GetCharacterAbilitySystemComponent()->CancelAbilitiesWithTag(BSTags::Ability_Player_Legs);
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
	FBSEquipmentMeshInfo NewInfo = FBSEquipmentMeshInfo(InSource);
	
	for (auto EquipmentMesh : InSource->EquipmentMeshes)
	{
		UBSEquipmentMeshComponent* NewMeshComp = NewObject<UBSEquipmentMeshComponent>(this, UBSEquipmentMeshComponent::StaticClass());
		if (!NewMeshComp) return;
		
		NewMeshComp->GetOnSkeletalMeshSet().AddWeakLambda(this, [this]
			(FGameplayTag MeshTag, USkeletalMesh* NewMesh)
		{
			OnSkeletalMeshSet.Broadcast(MeshTag, NewMesh);
		});
		
		NewMeshComp->InitializeEquipmentMesh(
			InSource->SlotTag, EquipmentMesh.SkeletalMesh, GetInventoryComponent()->GetPlayerColor());
		
		if (EquipmentMesh.bFollowPose)
		{
			NewMeshComp->SetLeaderPoseComponent(GetMesh());
		}
		else if (EquipmentMesh.AnimBP)
		{
			NewMeshComp->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
			NewMeshComp->SetAnimInstanceClass(EquipmentMesh.AnimBP);
		}
		
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
				OnSkeletalMeshRemoved.Broadcast(EquipmentMeshComponent->GetMeshTag(), EquipmentMeshComponent->GetSkeletalMeshAsset());
				EquipmentMeshComponent->DestroyComponent();	
			}
			It.RemoveCurrent();
			break;
		}
	}
}
