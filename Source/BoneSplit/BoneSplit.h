// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

DEFINE_LOG_CATEGORY_STATIC(BoneSplit, Log, All);

#define BS_HIT_DEBUG \
BSConsoleVariables::CVarBSDebugHitDetection.GetValueOnGameThread()
			                                         
UENUM(BlueprintType)
enum EBSAbilityInputID : uint8                         
{
	None = 0,
	Jump = 1,
	Special = 2,
	Soul = 3,
	Head = 4,
	PrimaryArm = 5,
	SecondaryArm = 6,
	Legs = 7
};
	
UENUM(BlueprintType)
enum class EBSReadyStatus : uint8
{
	Rs_Unknown     UMETA(DisplayName = "Unknown"),
	Rs_Ready       UMETA(DisplayName = "Ready"),
	Rs_NotReady    UMETA(DisplayName = "Not Ready"),
	Rs_TimedOut    UMETA(DisplayName = "Timed Out")
};
	
UENUM(BlueprintType)
enum class EBSPartyMode : uint8
{
	Party_Private = 0,
	Party_FriendsOnly = 1,
	Party_Open = 2
};

UENUM(BlueprintType)
enum class EBSMoveState : uint8
{
	Moving,
	Stopped,
	InvalidPath
};

UENUM(BlueprintType)
enum class EBSMovementRotationMode : uint8
{
	Ebs_Locked,
	Ebs_ControlRotation
};

namespace BSTags
{
	// =================================================================================================================
	// Gameplay Events
	// =================================================================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Knockback);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_DamageTaken);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_DamageDealt);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_HealingTaken);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_HealingDealt);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Aggro);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_KillingBlow);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_CombatStarted);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_CombatEnded);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_EquipmentRefreshed);
	
	// =================================================================================================================
	// Anim Events
	// =================================================================================================================
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimEvent_StoreCombo);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimEvent_Generic_01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimEvent_Generic_02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimEvent_Generic_03);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimEvent_Damage_01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimEvent_Damage_02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimEvent_Damage_03);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimEvent_Heal_01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimEvent_Heal_02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AnimEvent_Heal_03);

	// =================================================================================================================
	// Equipment Tags
	// =================================================================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Head);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Chest);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Arms);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Legs);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Curio);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Trinket);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Soul);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentMesh);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentMesh_Head);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentMesh_Chest);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentMesh_Arms);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentMesh_Legs);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentMesh_MainHand);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentMesh_Offhand);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponType);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponType_Daggers);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponType_Staff);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponType_SwordNBoard);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponType_Bow);
	
	// =================================================================================================================
	// Player Ability Tags
	// =================================================================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Head);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_PrimaryArm);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_SecondaryArm);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Legs);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Special);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Soul);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Player_Head);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Player_PrimaryArm);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Player_SecondaryArm);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Player_Legs);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Player_Soul);
	
	// =================================================================================================================
	// Talent Unlock Tags
	// =================================================================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Talent_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Talent_DoubleJump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Talent_Mine);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Talent_Grapple);
	
	// =================================================================================================================
	// Status Tags
	// =================================================================================================================
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Hurt);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Slowed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Stunned);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Dead);
	
	static TArray<FGameplayTag> StopMoveTags = { Status_Dead, Status_Stunned };
	
	// =================================================================================================================
	// Effect Tags
	// =================================================================================================================
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Persistent);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Crit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Saved);
	
	// =================================================================================================================
	// Season Tags
	// =================================================================================================================
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Season_Summer);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Season_Summer_Midsummer);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Season_Winter);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Season_Winter_Xmas);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Season_Fall_Halloween);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Season_Fall);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Season_Spring);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Season_Spring_Easter);

	// =================================================================================================================
	// Mob Ability Tags
	// =================================================================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Mob_BaseAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Mob_Special1);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Mob_Special2);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Mob_Special3);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Mob_Special4);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Mob_Special5);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Mob_Dodge);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Mob_BaseAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Mob_Special1);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Mob_Special2);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Mob_Special3);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Mob_Special4);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Mob_Special5);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability_Mob_Dodge);
	
	// =================================================================================================================
	// Faction Tags
	// =================================================================================================================
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faction);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faction_Player);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faction_Rats);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faction_Slimes);
	
}

namespace BSConsoleVariables
{
	extern TAutoConsoleVariable<float> CVarBSLookSensitivityX;
	extern TAutoConsoleVariable<float> CVarBSLookSensitivityY;
	extern TAutoConsoleVariable<float> CVarBSCameraOffset;
	extern TAutoConsoleVariable<bool> CVarBSBloomEnabled;
	
	extern TAutoConsoleVariable<bool> CVarBSDebugHitDetection;
	extern TAutoConsoleVariable<float> CVarBSDebugHitDetectionDuration;
	
	extern TAutoConsoleVariable<bool> CVarBSDebugAggroSpheres;
}