// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#include "Actors/Mob/BSMobCharacter.h"

#include "AIController.h"
#include "Actors/Mob/BSMobMovementComponent.h"
#include "Actors/Mob/BSMobSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/AbilitySystem/BSAbilityFunctionLibrary.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Components/FSM/BSFiniteState.h"
#include "Components/FSM/BSFiniteStateComponent.h"
#include "Components/Targeting/BSThreatComponent.h"
#include "GameInstance/BSPersistantDataSubsystem.h"

ABSMobCharacter::ABSMobCharacter(const FObjectInitializer& ObjectInitializer) : 
Super(ObjectInitializer.SetDefaultSubobjectClass<UBSMobMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationYaw = false;
	
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UBSAttributeSet>(TEXT("AttributeSet"));
	
	ThreatComponent = CreateDefaultSubobject<UBSThreatComponent>(TEXT("ThreatComponent"));
	ThreatComponent->OnCombatChanged.AddDynamic(this, &ABSMobCharacter::OnCombatChanged);
	
	StateMachineComponent = CreateDefaultSubobject<UBSFiniteStateComponent>(TEXT("StateMachineComponent"));
	
	GetReplicatedMovement_Mutable().RotationQuantizationLevel = ERotatorQuantization::ShortComponents;
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
}

void ABSMobCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	if (HasAuthority())
	{
		UBSPersistantDataSubsystem* DataSubsystem = UBSPersistantDataSubsystem::Get(this);
		check(DataSubsystem);
		int32 DifficultyLevel = DataSubsystem->GetDifficultyLevel();
		
		for (const auto DefaultAbility : GameplayAbility)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(DefaultAbility);
			AbilitySpec.Level = DifficultyLevel;
			AbilitySystemComponent->GiveAbility(AbilitySpec);
		}

		for (const auto DefaultEffect : Effects)
		{
			FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(
				DefaultEffect, DifficultyLevel, AbilitySystemComponent->MakeEffectContext());

			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
		}
		
		AbilitySystemComponent->AddLooseGameplayTags(GrantedTags, 1, EGameplayTagReplicationState::TagAndCountToAll);
	}
	else
	{
		StateMachineComponent->SetActive(false); //Server only comp
	}

	if (UBSMobSubsystem* MobSubsystem = GetWorld()->GetSubsystem<UBSMobSubsystem>())
	{
		MobSubsystem->RegisterMob(this);
	}
}

void ABSMobCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABSMobCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
		
	if (IdleState)
	{
		StateMachineComponent->ChangeState(IdleState);
	}
}

void ABSMobCharacter::LaunchActor(const FVector Direction, const float Magnitude)
{
	if (!HasAuthority()) return;
	
	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->AirControl = 0; //AI shouldn't have air control, let launches control movement
	
	GetAbilitySystemComponent()->CancelAbilities();
	
	LaunchCharacter(
	Direction * Magnitude, 
	true, 
	true);
}

void ABSMobCharacter::SetMovementRotationMode(const uint8 NewMovementMode)
{
	GetCharacterMovement()->bOrientRotationToMovement = 
		NewMovementMode == static_cast<uint8>(EBSMovementRotationMode::Ebs_Locked);
	
	GetCharacterMovement()->bUseControllerDesiredRotation = 
		NewMovementMode == static_cast<uint8>(EBSMovementRotationMode::Ebs_ControlRotation);
}

UAbilitySystemComponent* ABSMobCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABSMobCharacter::CheckAggroRadius()
{               
	if (!HasAuthority()) return;
	
	/*
	const bool bWasInCombat = GetThreatComponent()->IsInCombat();
	bool bNewCombat = bWasInCombat;
	TArray<AActor*> OverlappedActors = UBSAbilityFunctionLibrary::GetActorsInRadius(
		this, AggroSphereRadius, false);
	
#if WITH_EDITOR
	
	if (BSConsoleVariables::CVarBSDebugAggroSpheres.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), 
			GetActorLocation(), 
			AggroSphereRadius, 
			16,
			FColor::Yellow, 
			false, 
			AggroCheckFrequency);
	}
	
#endif
	
	for (const auto FoundActor : OverlappedActors)
	{
		if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(FoundActor))
		{
			if (UAbilitySystemComponent* OtherAsc = AscInterface->GetAbilitySystemComponent(); OtherAsc && 
				!UBSAbilityFunctionLibrary::FilterByNoMatchingFactions(GetAbilitySystemComponent(), OtherAsc)) 
				continue;
			
			if (!GetThreatComponent()->GetThreatMap().Contains(FoundActor))
			{
				if (UBSAbilityFunctionLibrary::CheckTargetVisibility(
					this, GetActorLocation(), FoundActor))
				{
					GetThreatComponent()->AddThreat(FoundActor, 1);
					bNewCombat = true;
				}
			}
		}
	}
	
	if (bWasInCombat != bNewCombat && CombatState)
	{
		StateMachineComponent->ChangeState(CombatState);
	}
	*/
}

UBSThreatComponent* ABSMobCharacter::GetThreatComponent()
{
	return ThreatComponent;
}

void ABSMobCharacter::OnCombatChanged(const bool bCombat)
{
	if (!HasAuthority()) return;
	if (bCombat)
	{
		StateMachineComponent->ChangeState(CombatState ? CombatState : nullptr);
	}
	else
	{
		StateMachineComponent->ChangeState(IdleState ? IdleState : nullptr);
	}
}

void ABSMobCharacter::OnKilled(AActor* Killer, float Damage)
{
	if (HasAuthority() && CanBeKilled())
	{
		Multicast_OnKilled(Killer, Damage);
	}
}

bool ABSMobCharacter::CanBeKilled() const
{
	return true;
}

void ABSMobCharacter::Destroyed()
{
	StateMachineComponent->ChangeState(nullptr);
	if (UBSMobSubsystem* MobSubsystem = GetWorld()->GetSubsystem<UBSMobSubsystem>())
	{
		MobSubsystem->UnRegisterMob(this);
	}
	
	Super::Destroyed();
}

void ABSMobCharacter::NativeOnCombatBegin()
{
	bIsInCombat = true;
}

void ABSMobCharacter::NativeOnCombatEnd()
{
	bIsInCombat = false;
}

void ABSMobCharacter::NativeOnCombatTick(const bool bReceivedToken, const float DeltaTime)
{
}

bool ABSMobCharacter::BP_IsInCombat_Implementation()
{
	return bIsInCombat;
}

float ABSMobCharacter::BP_GetAggroRange_Implementation()
{
	return AggroSphereRadius;
}

void ABSMobCharacter::Multicast_OnKilled_Implementation(AActor* Killer, const float Damage)
{
	OnEnemyKilledDelegate.Broadcast(Killer, Damage);
	Destroy();
}
