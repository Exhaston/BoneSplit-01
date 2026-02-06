// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#include "Actors/Mob/BSMobCharacter.h"

#include "AIController.h"
#include "Actors/Mob/BSMobMovementComponent.h"
#include "Actors/Mob/BSMobSubsystem.h"
#include "Animation/BSAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Components/Targeting/BSThreatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/BSAttributeBar.h"

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
	
	GetReplicatedMovement_Mutable().RotationQuantizationLevel = ERotatorQuantization::ShortComponents;
	
	GetMesh()->SetReceivesDecals(false);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WorldUIInfo"));
	WidgetComponent->SetupAttachment(GetMesh());
	WidgetComponent->SetRelativeLocation({0,0, 100});
	WidgetComponent->SetDrawSize({100, 20});
}

void ABSMobCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	GetCharacterMovement<UBSMobMovementComponent>()->InitializeAsc(AbilitySystemComponent);
	
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		if (UBSAnimInstance* BSAnimInstance = Cast<UBSAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			BSAnimInstance->InitializeAbilitySystemComponent(AbilitySystemComponent);
		}
	}
	
	if (HasAuthority())
	{
		SetRandomColor();
		int32 DifficultyLevel = 1;
		
		for (const auto& DefaultAbility : GameplayAbility)
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

	if (UBSMobSubsystem* MobSubsystem = GetWorld()->GetSubsystem<UBSMobSubsystem>())
	{
		MobSubsystem->RegisterMob(this);
	}
	
	if (!IsRunningDedicatedServer())
	{
		if (UBSAttributeBar* Bar = Cast<UBSAttributeBar>(WidgetComponent->GetWidget()))
		{
			Bar->InitializeAttributeBar(AbilitySystemComponent);
		}
	}
}

void ABSMobCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSMobCharacter, bIsInCombat);
	DOREPLIFETIME(ABSMobCharacter, bIsDead);
}

void ABSMobCharacter::LaunchCharacter(const FVector LaunchVelocity, const bool bXYOverride, const bool bZOverride)
{
	if (!HasAuthority()) return;
	
	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->AirControl = 0; //AI shouldn't have air control, let launches control movement
	
	GetAbilitySystemComponent()->CancelAbilities();
	
	Super::LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
}

void ABSMobCharacter::BP_OnLaunched_Implementation(FVector LaunchVelocity)
{
}

void ABSMobCharacter::Multicast_OnLaunched_Implementation(const FVector LaunchVelocity)
{
	Execute_BP_OnLaunched(this, LaunchVelocity);
}

void ABSMobCharacter::OnRep_Death()
{
	
}

void ABSMobCharacter::BP_OnDeath_Implementation(UAbilitySystemComponent* SourceAsc, float Damage)
{
}

UAbilitySystemComponent* ABSMobCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

FBSOnDeathDelegate& ABSMobCharacter::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

void ABSMobCharacter::Die(UAbilitySystemComponent* SourceAsc, float Damage)
{
	if (HasAuthority() && !bIsDead)
	{             
		if (GetController()) GetController()->UnPossess();
		bIsDead = true;
		GetCharacterMovement()->Velocity = FVector::Zero();
		
		if (!IsActorBeingDestroyed() && GetIsReplicated()) Multicast_OnDeath(SourceAsc, Damage);
	}
}

void ABSMobCharacter::Multicast_OnDeath_Implementation(UAbilitySystemComponent* SourceAsc, float Damage)
{
	//Stop further overlaps with this component
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetGenerateOverlapEvents(false);
	
	if (SourceAsc && SourceAsc->GetAvatarActor())
	{
		FVector LookDir = SourceAsc->GetAvatarActor()->GetActorLocation() - GetActorLocation();
		LookDir.Normalize();
		SetActorRotation({0, LookDir.ToOrientationRotator().Yaw, 0});
	}
	
	if (OnDeathDelegate.IsBound())
	{
		OnDeathDelegate.Broadcast(SourceAsc, GetAbilitySystemComponent(), Damage);	
	}
	
	Execute_BP_OnDeath(this, SourceAsc, Damage);
	
	if (GetMesh() && GetMesh()->GetAnimInstance() && !DeathAnimations.IsEmpty())
	{
		UAnimMontage* MontageToPlay = DeathAnimations[FMath::RandRange(0, DeathAnimations.Num() - 1)];
		float DeathTime = PlayAnimMontage(MontageToPlay, 1);
		
		GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, [this]()
		{
			Destroy();
		}, DeathTime, false);
	}
	else
	{
		Destroy();
	}
}

UBSThreatComponent* ABSMobCharacter::GetThreatComponent()
{
	return ThreatComponent;
}

bool ABSMobCharacter::IsInCombat()
{
	return bIsInCombat;
}

void ABSMobCharacter::OnCombatChanged(const bool bCombat)
{
}

void ABSMobCharacter::Launch(const FVector LaunchMagnitude, const bool bAdditive)
{
	if (HasAuthority())
	{
		LaunchCharacter(LaunchMagnitude, !bAdditive, !bAdditive);
		
		Multicast_OnLaunched(LaunchMagnitude);
	}
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

void ABSMobCharacter::BP_OnCombatChanged_Implementation(bool InCombat)
{
}

float ABSMobCharacter::BP_GetAggroRange_Implementation()
{
	return AggroSphereRadius;
}

void ABSMobCharacter::SetRandomColor()
{
	if (!ColorVariations.IsEmpty() && HasAuthority())
	{
		RandomColor = FMath::RandRange(0, ColorVariations.Num() - 1);
		OnRep_RandomColor();
	}
}

void ABSMobCharacter::OnRep_RandomColor()
{
	if (RandomColor != -1 &&  ColorVariations.IsValidIndex(RandomColor))
	{
		GetMesh()->SetVectorParameterValueOnMaterials("Color", FVector(ColorVariations[RandomColor]));
	}
}

void ABSMobCharacter::OnRep_OnCombatChanged()
{
	OnCombatChangedDelegate.Broadcast(bIsInCombat);
	Execute_BP_OnCombatChanged(this, bIsInCombat);
}

FBSOnCombatChangedDelegate& ABSMobCharacter::GetOnCombatChangedDelegate()
{
	return OnCombatChangedDelegate;
}
