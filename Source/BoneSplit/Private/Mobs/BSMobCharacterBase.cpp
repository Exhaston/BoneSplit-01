// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Mobs/BSMobCharacterBase.h"

#include "Abilities/BSAbilitySystem.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "Abilities/BSGameplayAbilitiesLibrary.h"
#include "AnimInstance/MMobAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Environment/BSBreakable.h"
#include "GameFramework/GameStateBase.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "Mobs/BSMobControllerBase.h"
#include "Mobs/BSMobMovementComponent.h"
#include "Mobs/BSMobSubsystem.h"
#include "Mobs/Abilities/BSMobAbilityChooserComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/HUD/BSUnitPlateComponent.h"
#include "Player/HUD/BSWidget_UnitPlate.h"
#include "Player/PowerUps/BSPowerUpComponent.h"
#include "Projectiles/BSProjectileSpawnerComponent.h"


ABSMobCharacterBase::ABSMobCharacterBase(const FObjectInitializer& ObjectInitializer) : 
Super(ObjectInitializer.SetDefaultSubobjectClass<UBSMobMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
	AIControllerClass = ABSMobControllerBase::StaticClass();
	
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystem>("AbilitySystemComponent");
	//Might set to minimal if net traffic is too much
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full); 
	AbilitySystemComponent->SetIsReplicated(true);
	
	AttributeSet = CreateDefaultSubobject<UBSExtendedAttributeSet>("AttributeSet");
	
	UnitPlateComponent = CreateDefaultSubobject<UBSUnitPlateComponent>("UnitPlateComponent");
	UnitPlateComponent->SetupAttachment(GetMesh());
	UnitPlateComponent->SetRelativeLocation(FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f);
	
	if (const UBSDeveloperSettings* DeveloperSettings = UBSDeveloperSettings::GetProjectSettings())
	{
		if (!DeveloperSettings->DefaultUnitPlate.IsNull() && UnitPlateComponent->GetWidgetClass() == nullptr)
		{
			UnitPlateComponent->SetWidgetClass(DeveloperSettings->DefaultUnitPlate.LoadSynchronous());
		}
	}
	
	MobAbilityChooserComponent = CreateDefaultSubobject<UBSMobAbilityChooserComponent>("MobAbilityChooserComponent");
	
	ProjectileSpawnerComponent = CreateDefaultSubobject<UBSProjectileSpawnerComponent>("ProjectileSpawnerComponent");
	
	TargetComponent = CreateDefaultSubobject<USceneComponent>("TargetComponent");
	TargetComponent->SetupAttachment(GetMesh());
}

void ABSMobCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSMobCharacterBase, FactionTags);
}

void ABSMobCharacterBase::ReInitCharacterData()
{
	if (CharacterInitData)
	{
		UBSMobSubsystem* MobSubsystem = UBSMobSubsystem::GetMobSubsystem(this);
		UBSGameplayAbilitiesLibrary::ApplyCharacterAttributes(CharacterInitData, AbilitySystemComponent, MobSubsystem->GetCurrentDifficulty());
	}
}

void ABSMobCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (const UBSDeveloperSettings* DeveloperSettings = UBSDeveloperSettings::GetProjectSettings())
	{
		if (!DeveloperSettings->DefaultUnitPlate.IsNull() && 
		(!UnitPlateComponent->GetWidgetClass()->IsChildOf(UBSWidget_UnitPlate::StaticClass()) 
			|| UnitPlateComponent->GetWidgetClass() == nullptr))
		{
			UnitPlateComponent->SetWidgetClass(DeveloperSettings->DefaultUnitPlate.LoadSynchronous());
		}
	}
}

void ABSMobCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (UBSMobSubsystem* MobSubsystem = UBSMobSubsystem::GetMobSubsystem(this); 
		MobSubsystem && CharacterInitData)
	{
		FactionTags.AddTag(BSGameplayTags::Faction_Breakable);
		
		UBSGameplayAbilitiesLibrary::ApplyCharacterDataTo(
			CharacterInitData, AbilitySystemComponent, MobSubsystem->GetCurrentDifficulty());
		
		MobSubsystem->RegisterMob(this);
		
		if (ABSMobControllerBase* MobControllerBase = Cast<ABSMobControllerBase>(NewController))
		{
			MobControllerBase->GetOnTargetChangedDelegate().AddWeakLambda(
				this, [this](AActor* OldTarget, AActor* NewTarget)
			{
				if (NewTarget) StartCombat();
			});
		}
	}
	else
	{
		UE_LOG(BoneSplit, Error, TEXT("Mob spawned with no init data %s"), *GetName())
	}
}

void ABSMobCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	UnitPlateComponent->SetupUnitPlate(AbilitySystemComponent, MobName);
	UnitPlateComponent->SetActive(true);
	if (HasAuthority())
	{
		AbilitySystemComponent->OnHealthZero.AddWeakLambda(this, [this]
			(UAbilitySystemComponent*, UAbilitySystemComponent*, FGameplayTagContainer, float, float)
		{
			NetMulticast_MobDied();
			
			if (UKismetMathLibrary::RandomBoolWithWeight(0.75f))
			{
				if (GetWorld())
				{
					for (auto PS : GetWorld()->GetGameState()->PlayerArray)
					{
						if (UBSPowerUpComponent* PowerUpComponent = UBSPowerUpComponent::GetPowerUpComponent(PS))
						{
							PowerUpComponent->SpawnPowerUpForPlayer(GetActorLocation(), FMath::RandRange(1, 3));
						}
					}
				}
			}
		});
	}
}

FVector ABSMobCharacterBase::GetTargetLocation(AActor* RequestedBy) const
{
	return TargetComponent->GetComponentLocation();
}

void ABSMobCharacterBase::NetMulticast_MobDied_Implementation()
{
	if (bIsDead) return;
	bIsDead = true;
	
	if (UnitPlateComponent)
	{
		UnitPlateComponent->DestroyComponent();
	}
	
	AbilitySystemComponent->ClearActorInfo();
	AbilitySystemComponent->DestroyActiveState();
	
	if (HasAuthority())
	{
		MobAbilityChooserComponent->StopAbilityChooser();
		
		GetCharacterMovement()->bUseRVOAvoidance = false;
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->StopMovementImmediately();
			
		DetachFromControllerPendingDestroy();
		TearOff();
	}
	
	GetWorldTimerManager().SetTimer(TimerHandle_LifeSpanExpired, this, &AActor::LifeSpanExpired, 60);
	
	OnMobDiedDelegate.Broadcast(this);
	
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (UAnimMontage* DeathMontage = UBSGameplayAbilitiesLibrary::GetRandomArrayElem(DeathMontages))
		{
			AnimInstance->Montage_Play(DeathMontage);
		}
	}
	
	AlignRotationToSurface();
}

void ABSMobCharacterBase::AlignRotationToSurface()
{
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 1000.f); // Trace downward

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
	{
		// Build rotation from surface normal
		FRotator NewRotation = FRotationMatrix::MakeFromZX(Hit.Normal, GetActorForwardVector()).Rotator();
		SetActorRotation(NewRotation);
		SetActorLocation(Hit.ImpactPoint + Hit.Normal * GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	}
}

UAbilitySystemComponent* ABSMobCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABSMobCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	return AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
}

bool ABSMobCharacterBase::HasMatchingGameplayTag(const FGameplayTag TagToCheck) const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(TagToCheck);
}

bool ABSMobCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AbilitySystemComponent->HasAllMatchingGameplayTags(TagContainer);
}

bool ABSMobCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AbilitySystemComponent->HasAnyMatchingGameplayTags(TagContainer);
}

FGameplayTagContainer& ABSMobCharacterBase::GetFactionTags()
{
	return FactionTags;
}

bool ABSMobCharacterBase::HasAnyMatchingFactionTag(const FGameplayTagContainer InFactionTags)
{
	return FactionTags.HasAnyExact(InFactionTags);
}

void ABSMobCharacterBase::StartCombat()
{
	if (!HasAuthority()) return;
	if (bInCombat) return;
	bInCombat = true;
	MobAbilityChooserComponent->StartAbilityChooser(AbilitySystemComponent);
}

UMMobAnimInstance* ABSMobCharacterBase::TryGetMobInstance()
{
	if (!GetMesh() || !GetMesh()->GetAnimInstance()) return nullptr;
	if (UMMobAnimInstance* MobAnimInstance = Cast<UMMobAnimInstance>(GetMesh()->GetAnimInstance())) 
		return MobAnimInstance;
	return nullptr;
}

void ABSMobCharacterBase::OnRep_FactionTags()
{
}
