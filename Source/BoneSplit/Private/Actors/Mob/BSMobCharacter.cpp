// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#include "Actors/Mob/BSMobCharacter.h"

#include "AIController.h"
#include "Actors/Mob/BSMobMovementComponent.h"
#include "Components/AbilitySystem/BSAbilityFunctionLibrary.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "GameInstance/BSPersistantDataSubsystem.h"
#include "Net/UnrealNetwork.h"

ABSMobCharacter::ABSMobCharacter(const FObjectInitializer& ObjectInitializer) : 
Super(ObjectInitializer.SetDefaultSubobjectClass<UBSMobMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationYaw = false;
	
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UBSAttributeSet>(TEXT("AttributeSet"));
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
	}
}

void ABSMobCharacter::LaunchActor(const FVector Direction, const float Magnitude)
{
	if (!HasAuthority()) return;
	
	if (GetAbilitySystemComponent()->GetAnimatingAbility() && IsPlayingRootMotion())
	{
		GetAbilitySystemComponent()->CurrentMontageStop();
	}
	
	GetCharacterMovement()->AirControl = 0; //AI shouldn't have air control, let launches control movement
	
	if (AAIController* AICont = GetController<AAIController>())
	{
		AICont->StopMovement();
	}
	
	LaunchCharacter(
	Direction.GetSafeNormal() * Magnitude, 
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
