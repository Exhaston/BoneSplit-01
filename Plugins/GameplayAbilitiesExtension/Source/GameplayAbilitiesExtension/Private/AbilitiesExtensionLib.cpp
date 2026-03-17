// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitiesExtensionLib.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitiesExtension.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace DefaultFactionTags
{
	UE_DEFINE_GAMEPLAY_TAG(Team, "Team");
	UE_DEFINE_GAMEPLAY_TAG(Team_Generic1, "Team.Generic1");
	UE_DEFINE_GAMEPLAY_TAG(Team_Generic2, "Team.Generic2");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faction, 
		"Faction", 
		"Parent tag only, please use a subtag. Adding more sub tags will allow for complex faction behaviours");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faction_Player, 
		"Faction.Player", 
		"Default Player tag.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faction_GenericEnemy, 
		"Faction.GenericEnemy", 
		"Use for any enemies that will be against the player and their allies.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faction_GenericFriendly, 
		"Faction.GenericFriendly", 
		"Generic tag for all enemies of the player and friendlies.");
}

namespace DefaultAbilityTags
{
	UE_DEFINE_GAMEPLAY_TAG(Ability, 
	"Ability");
	
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability, 
		"Cooldown.Ability");
}

FFactionTargetMode::FFactionTargetMode(const bool InSelf, const bool InEnemies, const bool InAllies)
{
	bSelf = InSelf;
	bEnemies = InEnemies;
	bAllies = InAllies;
}

bool FFactionTargetMode::EvaluateFaction(UAbilitySystemComponent* InComponent, UAbilitySystemComponent* InTarget)
{
	if (InComponent == InTarget) return bSelf;
	
	const bool bSharedFactions = 
		UAbilitiesExtensionLib::HasAnyMatchingSubTag(
		InComponent, 
		InTarget, 
		DefaultFactionTags::Faction);
	
	return (!bSharedFactions && bEnemies) || bSharedFactions && bAllies;
}

bool FFactionTargetMode::EvaluateFactionPvp(UAbilitySystemComponent* InComponent, UAbilitySystemComponent* InTarget)
{
	if (InComponent == InTarget) return bSelf;

	// In PVP, check for an explicit shared team tag rather than faction tags.
	// Players with no overlapping team tags are treated as enemies by default.
	const bool bSameTeam =
		UAbilitiesExtensionLib::HasAnyMatchingSubTag(
			InComponent,
			InTarget,
			DefaultFactionTags::Team); // e.g. "Team.Red", "Team.Blue"

	return (!bSameTeam && bEnemies) || (bSameTeam && bAllies);
}

float UAbilitiesExtensionLib::AsymptoticDr(const float InValue, const float InCap)
{
	if (InCap <= 0) return InValue;
	return InCap * (InValue / (InValue + InCap));
}

bool UAbilitiesExtensionLib::HasAnyMatchingSubTag(
	UAbilitySystemComponent* FirstAsc, UAbilitySystemComponent* SecondAsc, const FGameplayTag ParentTag)
{
	if (!FirstAsc || !SecondAsc)
	{
		FString DebugNullAsc;
		if (!FirstAsc) DebugNullAsc.Append(" First Ability System Input was Null! ");
		if (!SecondAsc) DebugNullAsc.Append(" Second Ability System Input was Null! ");
		UE_LOG(AbilitiesExtension, Error, TEXT("%s"), *DebugNullAsc)
		return false;
	}
	
	FGameplayTagContainer FilterContainer;
	FilterContainer.AddTagFast(ParentTag);

	const FGameplayTagContainer FilteredOwnerTags = FirstAsc->GetOwnedGameplayTags().Filter(FilterContainer);
	
	const FGameplayTagContainer FilteredOtherCompTags = SecondAsc->GetOwnedGameplayTags().Filter(FilterContainer);
	
	return FilteredOtherCompTags.HasAnyExact(FilteredOwnerTags);
}

bool UAbilitiesExtensionLib::IsActorMoving(AActor* InActor)
{
	if (!InActor)
	{
		UE_LOG(AbilitiesExtension, Error, TEXT("IsActorMoving executed with null actor!"))
		return false;
	}

	return !InActor->GetVelocity().IsNearlyZero();
}

bool UAbilitiesExtensionLib::IsCharacterGrounded(ACharacter* InCharacter)
{
	if (!InCharacter)                                                                
	{                                                                               
		UE_LOG(AbilitiesExtension, Error, TEXT("IsCharacterGrounded executed with null Character!"))
		return false;
	}
	return !InCharacter->GetCharacterMovement()->IsFalling();
}

EMovementMode UAbilitiesExtensionLib::GetCharacterMovementMode(ACharacter* InCharacter)
{
	if (!InCharacter)                                                                
	{                                                                               
		UE_LOG(AbilitiesExtension, Error, TEXT("GetCharacterMovementMode executed with null Character!"))
		return MOVE_None;
	}
	return InCharacter->GetCharacterMovement()->MovementMode;
}

int32 UAbilitiesExtensionLib::GetCharacterCustomMovementMode(ACharacter* InCharacter)
{
	if (!InCharacter)                                                                
	{                                                                               
		UE_LOG(AbilitiesExtension, Error, TEXT("GetCharacterCustomMovementMode executed with null Character!"))
		return -1;
	}
	return InCharacter->GetCharacterMovement()->CustomMovementMode;
}

