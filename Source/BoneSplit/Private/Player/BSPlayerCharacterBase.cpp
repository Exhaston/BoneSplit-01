// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/BSPlayerCharacterBase.h"

#include "AbilitySystemInterface.h"
#include "Abilities/BSAbilitySystem.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "Abilities/BSGameplayAbilitiesLibrary.h"
#include "BoneSplit/BoneSplit.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/Equipment/BSEquipmentComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/BSPlayerAnimInstance.h"
#include "Player/BSPlayerMovement.h"
#include "Player/BSPlayerStateBase.h"
#include "Player/HUD/BSScreenAlignedTextComponent.h"
#include "Player/Interactions/BSPlayerInteractionComponent.h"
#include "Projectiles/BSProjectileSpawnerComponent.h"

namespace BSConsoleVariables
{
	TAutoConsoleVariable<float> CVarBSCameraOffset(
		TEXT("BS.Player.CameraOffset"),
		1,
		TEXT("1 = Default"),
		ECVF_Default);
	
	TAutoConsoleVariable<float> CVarBSCameraFieldOfView(
		TEXT("BS.Player.FieldOfView"),
		90,
		TEXT("90 = Default"),
		ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarDebugNoEquipment(
		TEXT("BS.Debug.ToggleGear"),
		false,
		TEXT("0 = Default"),
		ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarBSShowOwnName(
	TEXT("BS.Player.ShowOwnName"),
	false,
	TEXT("0 = Default"),
	ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarBSShowOtherNames(
	TEXT("BS.Player.ShowOtherNames"),
	true,
	TEXT("1 = Default"),
	ECVF_Default);
}

ABSPlayerCharacterBase::ABSPlayerCharacterBase(const FObjectInitializer& ObjectInitializer) : 
Super(ObjectInitializer.SetDefaultSubobjectClass<UBSPlayerMovement>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	GetCapsuleComponent()->SetCapsuleHalfHeight(64);
	GetCapsuleComponent()->SetCapsuleRadius(32);
	
	SetNetUpdateFrequency(64);
	bAlwaysRelevant = true;
	SetReplicates(true);
	
	GetReplicatedMovement_Mutable().RotationQuantizationLevel = ERotatorQuantization::ShortComponents;
	
	GetMesh()->SetReceivesDecals(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	GetMesh()->SetTickableWhenPaused(false);
	GetMesh()->SetVisibility(false);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	
	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	AudioComponent->SetupAttachment(GetMesh());
	AudioComponent->bCanPlayMultipleInstances = true;
	AudioComponent->SetRelativeLocation({0,0,64});
	
	BSConsoleVariables::CVarDebugNoEquipment->OnChangedDelegate().AddWeakLambda(
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
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->SetRelativeLocation(FVector(0,0,100));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SocketOffset = FVector(0, 100, 25);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	bUseControllerRotationYaw = false;
	
	PlayerNameTextComponent = CreateDefaultSubobject<UBSScreenAlignedTextComponent>(TEXT("PlayerNameTextComponent"));
	PlayerNameTextComponent->SetupAttachment(GetMesh());
	PlayerNameTextComponent->SetText(FText::FromString("PlayerName"));
	
	PlayerNameTextComponent->SetRelativeLocation(FVector(0,0,GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.25f));
	
	ProjectileSpawnerComponent = 
		CreateDefaultSubobject<UBSProjectileSpawnerComponent>(TEXT("ProjectileSpawnerComponent"));
	
	EquipmentComponent = CreateDefaultSubobject<UBSEquipmentComponent>(TEXT("EquipmentComponent"));
	
	TargetComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TargetComponent"));
	TargetComponent->SetupAttachment(GetMesh());
	
	PlayerInteractionComponent = CreateDefaultSubobject<UBSPlayerInteractionComponent>(TEXT("PlayerInteractionComponent"));
}

void ABSPlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSPlayerCharacterBase, FactionTags);
}

void ABSPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitAbilitySystem();
}

void ABSPlayerCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();
	
	InitAbilitySystem();
}

void ABSPlayerCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitAbilitySystem();
}

void ABSPlayerCharacterBase::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);
	
	InitAbilitySystem();
}

void ABSPlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	InitAbilitySystem();
	
	SpringArmComponent->TargetArmLength = BSConsoleVariables::CVarBSCameraOffset.GetValueOnGameThread();
	BSConsoleVariables::CVarBSCameraOffset->OnChangedDelegate().AddWeakLambda(
	this, [this](IConsoleVariable* ConsoleVariable)
	{
		SpringArmComponent->TargetArmLength = ConsoleVariable->GetFloat();
	});
	
	CameraComponent->SetFieldOfView(BSConsoleVariables::CVarBSCameraFieldOfView.GetValueOnGameThread());
	BSConsoleVariables::CVarBSCameraFieldOfView->OnChangedDelegate().AddWeakLambda(
	this, [this](IConsoleVariable* ConsoleVariable)
	{
		CameraComponent->SetFieldOfView(ConsoleVariable->GetFloat());
	});
}

bool ABSPlayerCharacterBase::CanJumpInternal_Implementation() const
{
	bool bJumpIsAllowed = GetCharacterMovement()->CanAttemptJump();
    
	const bool bCoyoteActive = 
		GetCharacterMovement<UBSPlayerMovement>()->IsCoyoteTimeActive();

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

void ABSPlayerCharacterBase::InitAbilitySystem()
{
	APlayerState* PS = GetPlayerState();
	if (!PS) return;
	
	const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(PS);
	if (!AbilitySystemInterface)
	{
		UE_LOG(BoneSplit, Display, TEXT("Waiting for Player State %s"), *GetName())
		return; //First check for a player state.              
	}
	
	UAbilitySystemComponent* OwnerAbilitySystem = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!OwnerAbilitySystem)
	{
		UE_LOG(BoneSplit, Display, TEXT("Waiting for Ability System %s"), *GetName())
		return; //For some reason if ASC isn't replicated yet
	}
	
	//Already set
	if (OwnerAbilitySystem == AbilitySystemComponent && AbilitySystemComponent->GetAvatarActor() == this) return; 
	
	if (HasAuthority() || IsLocallyControlled())
	{
		if (!GetController())
		{                                               
			UE_LOG(BoneSplit, Display, TEXT("Waiting for Player Controller (Server and Client only) %s"), *GetName())
			return; //Check for a valid controller for non-simulated proxies
		}
	}
	
	AbilitySystemComponent = Cast<UBSAbilitySystem>(OwnerAbilitySystem);
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	
	UE_LOG(BoneSplit, Display, TEXT("%s Initialization completed successfully."), *GetName())
	bAbilitySystemInitialized = true;
	
	OnAbilitySystemInitComplete();
}

UAbilitySystemComponent* ABSPlayerCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABSPlayerCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
}

bool ABSPlayerCharacterBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(TagToCheck);
}

bool ABSPlayerCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AbilitySystemComponent->HasAllMatchingGameplayTags(TagContainer);
}

bool ABSPlayerCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AbilitySystemComponent->HasAnyMatchingGameplayTags(TagContainer);
}

FGameplayTagContainer& ABSPlayerCharacterBase::GetFactionTags()
{
	return FactionTags;
}

bool ABSPlayerCharacterBase::HasAnyMatchingFactionTag(const FGameplayTagContainer InFactionTags)
{
	return FactionTags.HasAnyExact(InFactionTags);
}

void ABSPlayerCharacterBase::SetMenuCamera()
{
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->SetRelativeRotation({0, -45, 0});
}

void ABSPlayerCharacterBase::ResetCamera()
{
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetRelativeRotation({0, 90, 0});
}

void ABSPlayerCharacterBase::LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	if (HasAuthority() && !IsLocallyControlled())
	{
		Client_LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
	}
	
	Super::LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
}

void ABSPlayerCharacterBase::Client_LaunchCharacter_Implementation(FVector Direction, bool OverrideXY, bool OverrideZ)
{	
	LaunchCharacter(Direction, OverrideXY, OverrideZ);
}

UBSPlayerAnimInstance* ABSPlayerCharacterBase::TryGetPlayerAnimInstance()
{
	if (!GetMesh()) return nullptr;
	if (UBSPlayerAnimInstance* PlayerAnimInstance = Cast<UBSPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		return PlayerAnimInstance;
	}
	return nullptr;
}

void ABSPlayerCharacterBase::OnAbilitySystemInitComplete()
{
	EquipmentComponent->SetEquipmentOwner(this, GetAbilitySystemComponent());
	GetCharacterMovement<UBSPlayerMovement>()->SetupPlayerMovement(GetAbilitySystemComponent());
	SetupPlayerName();
	
	GetAbilitySystemComponent()->AddGameplayEventTagContainerDelegate(
		FGameplayTagContainer(BSExtendedAttributeTags::GameplayEvent_SuccessfulBlock),
		FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
			this,
			&ABSPlayerCharacterBase::OnSuccessfulBlock
		)
	);
	
	GetAbilitySystemComponent()->RegisterGenericGameplayTagEvent().AddWeakLambda(
		this, [this] (FGameplayTag GameplayTag, int32 NewCount)
	{
		if (UBSPlayerAnimInstance* AnimInstance = TryGetPlayerAnimInstance())
		{
			if (NewCount <= 0)
				AnimInstance->NativeOnTagRemoved(GameplayTag);
			else
				AnimInstance->NativeOnTagAdded(GameplayTag);
		}
	});
	
	EquipmentComponent->GetOnEquipmentAppliedDelegate().AddWeakLambda(
	this, [this] (const FBSEquipmentInstance& EquipmentInstance)
	{
		if (UBSPlayerAnimInstance* AnimInstance = TryGetPlayerAnimInstance())
		{
			//AnimInstance->BP_OnEquipmentAdded(EquipmentInstance.GetDef())
		}
	});
	
	
	
	if (HasAuthority())
	{
		EquipmentComponent->ApplyDefaultEquipment();
	
		if (ABSPlayerStateBase* PS = GetPlayerState<ABSPlayerStateBase>(); PS && !PS->GetHasAbilitySystemData())
		{
			if (CharacterInitData) 
				UBSGameplayAbilitiesLibrary::ApplyCharacterDataTo(CharacterInitData, AbilitySystemComponent, 1);
		}
	}
	
	OnAbilitySystemInitializedDelegate.Broadcast();
}

void ABSPlayerCharacterBase::SetupPlayerName()
{
	const APlayerState* PS = GetPlayerState();
	if (!PS) return;
	if (!IsRunningDedicatedServer())
	{
		PlayerNameTextComponent->SetText(FText::FromString(PS->GetPlayerName()));
		
		if (!IsLocallyControlled())
		{	
			PlayerNameTextComponent.Get()->SetHiddenInGame(!BSConsoleVariables::CVarBSShowOtherNames->GetBool());
			BSConsoleVariables::CVarBSShowOtherNames->OnChangedDelegate().AddWeakLambda(
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
			PlayerNameTextComponent.Get()->SetHiddenInGame(!BSConsoleVariables::CVarBSShowOwnName->GetBool());
			BSConsoleVariables::CVarBSShowOwnName->OnChangedDelegate().AddWeakLambda(
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

FVector ABSPlayerCharacterBase::GetTargetLocation(AActor* RequestedBy) const
{
	return TargetComponent->GetComponentLocation();
}

void ABSPlayerCharacterBase::OnSuccessfulBlock(FGameplayTag MatchingTag, const FGameplayEventData* EventData)
{
	if (BlockMontage)
		PlayAnimMontage(BlockMontage);
}

void ABSPlayerCharacterBase::OnRep_FactionTags()
{
}





