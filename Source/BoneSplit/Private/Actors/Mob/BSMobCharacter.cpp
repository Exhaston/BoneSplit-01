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
#include "Components/Targeting/BSAggroComponent.h"
#include "Components/Targeting/BSThreatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Tasks/AITask_MoveTo.h"
#include "Widgets/HUD/BSFloatingNamePlate.h"
#include "Widgets/HUD/FloatingUnitPlates/BSFloatingUnitPlateSubsystem.h"

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
	
	ThreatComponent->OnMaxThreatChanged.AddDynamic(this, &ABSMobCharacter::OnThreatTargetUpdate);
	
	
	GetReplicatedMovement_Mutable().RotationQuantizationLevel = ERotatorQuantization::ShortComponents;
	
	GetMesh()->SetReceivesDecals(false);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetMesh());
	
	AggroComponent = CreateDefaultSubobject<UBSAggroComponent>(TEXT("AggroComponent"));
	AggroComponent->OnTargetFoundDelegate.AddDynamic(this, &ABSMobCharacter::OnTargetFound);
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

		for (auto GrantedAbilities : AbilityWeightMap)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(GrantedAbilities.Key);
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
		MobSubsystem->DeclareMob(this);
	}
	
	if (!IsRunningDedicatedServer())
	{
		if (UBSFloatingNamePlate* FloatingNamePlate = Cast<UBSFloatingNamePlate>(WidgetComponent->GetWidget()))
		{
			FloatingNamePlate->InitializeAbilitySystemComponent(AbilitySystemComponent);
		}
	
		if (UBSFloatingUnitPlateSubsystem* FloatingUnitPlateSubsystem = GetWorld()->GetSubsystem<UBSFloatingUnitPlateSubsystem>())
		{
			FloatingUnitPlateSubsystem->RegisterFloatingNamePlate(WidgetComponent);
		}
	}
}

void ABSMobCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveAll(
			this);
	}
	
	Super::EndPlay(EndPlayReason);
	

}

void ABSMobCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!IsValid(this) || !HasAuthority() || GetTearOff() || !GetAbilitySystemComponent() || !GetThreatComponent()) return;
	
	ElapsedAbilityCheckTime += DeltaSeconds;
	
	if (ElapsedAbilityCheckTime >= AbilityCheckTimeInterval)
	{
		ElapsedAbilityCheckTime = 0.0f;
		if (AbilityWeightMap.IsEmpty()) return;
		if (GetAbilitySystemComponent()->HasMatchingGameplayTag(BSTags::Status_Dead)) return;
		if (!GetThreatComponent()->GetHighestThreatActor()) return;
		
		if (const TSubclassOf<UGameplayAbility> ActivatedAbility = 
		UBSAbilityLibrary::GetAbilityByWeight(GetAbilitySystemComponent(), AbilityWeightMap))
		{
			GetAbilitySystemComponent()->TryActivateAbilityByClass(ActivatedAbility);
		}
	}
}

void ABSMobCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSMobCharacter, bIsInCombat);
}

void ABSMobCharacter::OnTargetFound(AActor* InActor)
{
	if (!HasAuthority()) return;
	GetThreatComponent()->AddThreat(InActor, 1);
}

void ABSMobCharacter::OnThreatTargetUpdate(AActor* NewTarget, float NewThreat)
{
	MoveToTarget();
}

void ABSMobCharacter::MoveToTarget()
{
	if (!GetController()) return;
	CurrentMoveTask = UAITask_MoveTo::AIMoveTo(GetController<AAIController>(), {}, GetThreatComponent()->GetHighestThreatActor(), FollowDistance, EAIOptionFlag::Disable, EAIOptionFlag::Default, true, false);
	
	CurrentMoveTask->OnMoveTaskFinished.AddWeakLambda(this, [this]
	(TEnumAsByte<EPathFollowingResult::Type> MoveTaskResult, AAIController* AIController)
	{
		if (!GetWorld()) return;
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, MoveTaskResult]()
		{
			if (!IsValid(this)) return;
			if (MoveTaskResult == EPathFollowingResult::Success)
			{
				bInRange = true;
				MoveToTarget();
			}
			else
			{
				bInRange = false;
				MoveToTarget();
			}
		});
	});
	
	CurrentMoveTask->ReadyForActivation();
}

void ABSMobCharacter::BP_OnNewTarget_Implementation(AActor* NewTarget)
{
}

void ABSMobCharacter::LaunchCharacter(const FVector LaunchVelocity, const bool bXYOverride, const bool bZOverride)
{
	if (!HasAuthority()) return;
	

	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->AirControl = 0; //AI shouldn't have air control, let launches control movement
	
	GetAbilitySystemComponent()->CancelAllAbilities();
	StopAnimMontage();
	Super::LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
}

void ABSMobCharacter::BP_OnLaunched_Implementation(FVector LaunchVelocity)
{
}

void ABSMobCharacter::Multicast_OnLaunched_Implementation(const FVector LaunchVelocity)
{
	Execute_BP_OnLaunched(this, LaunchVelocity);
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
	if (HasAuthority() && GetAbilitySystemComponent() && !GetAbilitySystemComponent()->HasMatchingGameplayTag(BSTags::Status_Dead))
	{             
		SetActorEnableCollision(false);
		GetAbilitySystemComponent()->AddLooseGameplayTag(BSTags::Status_Dead, 1, EGameplayTagReplicationState::TagAndCountToAll);
		GetCharacterMovement()->DisableMovement();
		
		if (CurrentMoveTask)
		{
			CurrentMoveTask->EndTask();
		}
		
		GetAbilitySystemComponent()->CancelAllAbilities();
		GetCharacterMovement()->Velocity = FVector::Zero();
		
		if (GetController()) GetController()->UnPossess();
		
		if (!DeathAnimations.IsEmpty())
		{
			ActiveDeathMontage = DeathAnimations[FMath::RandRange(0, DeathAnimations.Num() - 1)];
			CommitDeath(ActiveDeathMontage);
		}
	}
}

void ABSMobCharacter::CommitDeath_Implementation(UAnimMontage* Montage)
{
	ActiveDeathMontage = Montage;
	
	if (!IsRunningDedicatedServer())
	{
		if (UBSFloatingNamePlate* FloatingNamePlate = Cast<UBSFloatingNamePlate>(WidgetComponent->GetWidget()))
		{
			FloatingNamePlate->StartDeathAnimation();
		}
	}
	
	if (Montage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		FOnMontageBlendingOutStarted BlendEnded = FOnMontageBlendingOutStarted::CreateWeakLambda(this, [this](UAnimMontage* Montage, bool bInterrupted)
		{
			OnDeathMontageEnded(Montage, bInterrupted);
		});
		
		GetMesh()->GetAnimInstance()->Montage_Play(Montage);
		
		GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendEnded, Montage);
	}
	
	if (HasAuthority())
	{
		TearOff();
	}
}

void ABSMobCharacter::OnDeathMontageEnded(UAnimMontage* Montage, bool bFinished)
{
    if (!IsValid(this))
        return;
	
	if (Montage != ActiveDeathMontage) return;

	if (const UWorld* World = GetWorld(); !World || World->bIsTearingDown) return;
	
	
	Destroy(true);
}

void ABSMobCharacter::Multicast_OnDeath_Implementation(UAbilitySystemComponent* SourceAsc, float Damage, UAnimMontage* Montage)
{
	//Stop further overlaps with this component
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetGenerateOverlapEvents(false);
	StopAnimMontage();
	
	if (OnDeathDelegate.IsBound())
	{
		OnDeathDelegate.Broadcast(SourceAsc, GetAbilitySystemComponent(), Damage);	
	}
	
	Execute_BP_OnDeath(this, SourceAsc, Damage);
	

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
