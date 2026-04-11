// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Environment/BSBreakable.h"

#include "CharacterAbilitySystem.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PowerUps/BSPowerUpComponent.h"

namespace BSGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Faction_Breakable, "Faction.Breakable")
}

ABSBreakable::ABSBreakable(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetNetUpdateFrequency(8);
	bAlwaysRelevant = true;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
	
	CharacterAbilitySystem = CreateDefaultSubobject<UCharacterAbilitySystem>(TEXT("CharacterAbilitySystem"));
	CharacterAbilitySystem->SetIsReplicated(true);
	CharacterAbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UBSExtendedAttributeSet>(TEXT("AttributeSet"));
}

void ABSBreakable::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority() && !UKismetMathLibrary::RandomBoolWithWeight(BreakableSpawnChance))
	{
		Destroy();
		return;
	}
	
	CharacterAbilitySystem->InitAbilityActorInfo(this, this);
	CharacterAbilitySystem->OnHealthZero.AddUObject(this, &ABSBreakable::OnDeath);
	
	if (HasAuthority())
	{
		CharacterAbilitySystem->ApplyModToAttribute(UBSExtendedAttributeSet::GetCurrentHealthAttribute(), EGameplayModOp::Override, Health);
		CharacterAbilitySystem->ApplyModToAttribute(UBSExtendedAttributeSet::GetMaxHealthAttribute(), EGameplayModOp::Override, Health);
	}
}

UAbilitySystemComponent* ABSBreakable::GetAbilitySystemComponent() const
{
	return CharacterAbilitySystem;
}

bool ABSBreakable::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return CharacterAbilitySystem->HasMatchingGameplayTag(TagToCheck);
}

bool ABSBreakable::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return CharacterAbilitySystem->HasAllMatchingGameplayTags(TagContainer);
}

bool ABSBreakable::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return CharacterAbilitySystem->HasAllMatchingGameplayTags(TagContainer);
}

void ABSBreakable::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	return CharacterAbilitySystem->GetOwnedGameplayTags(TagContainer);
}

bool ABSBreakable::HasAnyMatchingFactionTag(FGameplayTagContainer InFactionTags)
{
	return FactionTags.HasAnyExact(InFactionTags);
}

void ABSBreakable::BP_OnDeath_Implementation()
{
}

void ABSBreakable::OnDeath(UAbilitySystemComponent*, UAbilitySystemComponent*, FGameplayTagContainer, float, float)
{
	SetActorEnableCollision(false);
	
	if (GetWorld() && HasAuthority() && GetWorld()->GetGameState())
	{
		for (int i = 0; i < MaxSpawns; ++i)
		{
			for (auto PS : GetWorld()->GetGameState()->PlayerArray)
			{
				if (UBSPowerUpComponent* PowerUpComponent = UBSPowerUpComponent::GetPowerUpComponent(PS))
				{
					if (UKismetMathLibrary::RandomBoolWithWeight(SpawnChance))
					{
						FVector SpawnOffset = FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 50);
						PowerUpComponent->SpawnPowerUpForPlayer(GetActorLocation() + SpawnOffset, 3);
					}
				}
			}
		}
	}
	
	BP_OnDeath();
}
