// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoneSplit.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, BoneSplit, "BoneSplit" );

namespace BSTags
{
	// =================================================================================================================
	// Gameplay Events
	// =================================================================================================================

	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Knockback, "GameplayEvent.Knockback");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_DamageTaken, "GameplayEvent.DamageTaken");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_DamageDealt, "GameplayEvent.DamageDealt");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_HealingTaken, "GameplayEvent.HealingTaken");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_HealingDealt, "GameplayEvent.HealingDealt");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Aggro, "GameplayEvent.Aggro");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_KillingBlow, "GameplayEvent.KillingBlow");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Death, "GameplayEvent.Death");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_CombatStarted, "GameplayEvent.CombatStarted");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_CombatEnded, "GameplayEvent.CombatEnded");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_EquipmentRefreshed, "GameplayEvent.EquipmentRefreshed");
	
	// =================================================================================================================
	// Anim Events
	// =================================================================================================================
	
	UE_DEFINE_GAMEPLAY_TAG(AnimEvent_StoreCombo, "AnimEvent.StoreCombo");
	
	UE_DEFINE_GAMEPLAY_TAG(AnimEvent_Generic_01, "AnimEvent.Generic.01");
	UE_DEFINE_GAMEPLAY_TAG(AnimEvent_Generic_02, "AnimEvent.Generic.02");
	UE_DEFINE_GAMEPLAY_TAG(AnimEvent_Generic_03, "AnimEvent.Generic.03");
	
	UE_DEFINE_GAMEPLAY_TAG(AnimEvent_Damage_01, "AnimEvent.Damage.01");
	UE_DEFINE_GAMEPLAY_TAG(AnimEvent_Damage_02, "AnimEvent.Damage.02");
	UE_DEFINE_GAMEPLAY_TAG(AnimEvent_Damage_03, "AnimEvent.Damage.03");
	
	UE_DEFINE_GAMEPLAY_TAG(AnimEvent_Heal_01, "AnimEvent.Heal.01");
	UE_DEFINE_GAMEPLAY_TAG(AnimEvent_Heal_02, "AnimEvent.Heal.02");
	UE_DEFINE_GAMEPLAY_TAG(AnimEvent_Heal_03, "AnimEvent.Heal.03");
		 
	// =================================================================================================================
	// Equipment Tags
	// =================================================================================================================

	UE_DEFINE_GAMEPLAY_TAG(Equipment, "Equipment");
	
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Head, "Equipment.Head");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Chest, "Equipment.Chest");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Arms, "Equipment.Arms");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Legs, "Equipment.Legs");
	
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Curio, "Equipment.Curio");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Trinket, "Equipment.Trinket");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Soul, "Equipment.Soul");
	
	UE_DEFINE_GAMEPLAY_TAG(EquipmentMesh, "EquipmentMesh");
	
	UE_DEFINE_GAMEPLAY_TAG(EquipmentMesh_Head, "EquipmentMesh.Head");
	UE_DEFINE_GAMEPLAY_TAG(EquipmentMesh_Chest, "EquipmentMesh.Chest");
	UE_DEFINE_GAMEPLAY_TAG(EquipmentMesh_Arms, "EquipmentMesh.Arms");
	UE_DEFINE_GAMEPLAY_TAG(EquipmentMesh_Legs, "EquipmentMesh.Legs");
	UE_DEFINE_GAMEPLAY_TAG(EquipmentMesh_MainHand, "EquipmentMesh.MainHand");
	UE_DEFINE_GAMEPLAY_TAG(EquipmentMesh_Offhand, "EquipmentMesh.Offhand");
	
	UE_DEFINE_GAMEPLAY_TAG(WeaponType, "WeaponType");
	UE_DEFINE_GAMEPLAY_TAG(WeaponType_Daggers, "WeaponType.Daggers");
	UE_DEFINE_GAMEPLAY_TAG(WeaponType_Staff, "WeaponType.Staff");
	UE_DEFINE_GAMEPLAY_TAG(WeaponType_SwordNBoard, "WeaponType.SwordNBoard");
	UE_DEFINE_GAMEPLAY_TAG(WeaponType_Bow, "WeaponType.Bow");

	// =================================================================================================================
	// Player Ability Tags
	// =================================================================================================================

	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Head, "Ability.Player.Head");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_PrimaryArm, "Ability.Player.PrimaryArm");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_SecondaryArm, "Ability.Player.SecondaryArm");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Legs, "Ability.Player.Legs");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Soul, "Ability.Player.Soul");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Special, "Ability.Player.Special");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Jump, "Ability.Player.Jump");
	
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Player_Head, "Cooldown.Ability.Player.Head");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Player_PrimaryArm, "Cooldown.Ability.Player.PrimaryArm");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Player_SecondaryArm, "Cooldown.Ability.Player.SecondaryArm");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Player_Legs, "Cooldown.Ability.Player.Legs");

	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Player_Soul, "Cooldown.Ability.Player.Soul");
	
	UE_DEFINE_GAMEPLAY_TAG(Talent_Jump, "Talent.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Talent_DoubleJump, "Talent.DoubleJump");
	UE_DEFINE_GAMEPLAY_TAG(Talent_Mine, "Talent.Mine");
	UE_DEFINE_GAMEPLAY_TAG(Talent_Grapple, "Talent.Grapple");
	
	// =================================================================================================================
	// Status Tags
	// =================================================================================================================
	
	UE_DEFINE_GAMEPLAY_TAG(Status_Hurt, "Status.Hurt");
	UE_DEFINE_GAMEPLAY_TAG(Status_Slowed, "Status.Slowed");
	UE_DEFINE_GAMEPLAY_TAG(Status_Stunned, "Status.Stunned");
	UE_DEFINE_GAMEPLAY_TAG(Status_Dead, "Status.Dead");
	
	// =================================================================================================================
	// Effect Tags
	// =================================================================================================================
	         
	UE_DEFINE_GAMEPLAY_TAG(Effect_Crit, "Effect.Crit");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Persistent, "Effect.Persistent");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Saved, "Effect.Saved");
	
	// =================================================================================================================
	// Season Tags
	// =================================================================================================================
	
	UE_DEFINE_GAMEPLAY_TAG(Season_Summer, "Season.Summer");
	UE_DEFINE_GAMEPLAY_TAG(Season_Summer_Midsummer, "Season.Summer.Midsummer");
	
	UE_DEFINE_GAMEPLAY_TAG(Season_Winter, "Season.Winter");
	UE_DEFINE_GAMEPLAY_TAG(Season_Winter_Xmas, "Season.Winter.Xmas");
	
	UE_DEFINE_GAMEPLAY_TAG(Season_Fall_Halloween, "Season.Fall.Halloween");
	UE_DEFINE_GAMEPLAY_TAG(Season_Fall, "Season.Fall");
	
	UE_DEFINE_GAMEPLAY_TAG(Season_Spring, "Season.Spring");
	UE_DEFINE_GAMEPLAY_TAG(Season_Spring_Easter, "Season.Spring.Easter");

	// =================================================================================================================
	// Mob Ability Tags
	// =================================================================================================================

	UE_DEFINE_GAMEPLAY_TAG(Ability_Mob_BaseAttack, "Ability.Mob.BaseAttack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Mob_Special1, "Ability.Mob.Special1");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Mob_Special2, "Ability.Mob.Special2");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Mob_Special3, "Ability.Mob.Special3");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Mob_Special4, "Ability.Mob.Special4");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Mob_Special5, "Ability.Mob.Special5");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Mob_Dodge, "Ability.Mob.Dodge");
	
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Mob_BaseAttack, "Cooldown.Ability.Mob.BaseAttack");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Mob_Special1, "Cooldown.Ability.Mob.Special1");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Mob_Special2, "Cooldown.Ability.Mob.Special2");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Mob_Special3, "Cooldown.Ability.Mob.Special3");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Mob_Special4, "Cooldown.Ability.Mob.Special4");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Mob_Special5, "Cooldown.Ability.Mob.Special5");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Mob_Dodge, "Cooldown.Ability.Mob.Dodge");
	
	UE_DEFINE_GAMEPLAY_TAG(Faction, "Faction");
	UE_DEFINE_GAMEPLAY_TAG(Faction_Player, "Faction.Player");
	UE_DEFINE_GAMEPLAY_TAG(Faction_Rats, "Faction.Rats");
	UE_DEFINE_GAMEPLAY_TAG(Faction_Slimes, "Faction.Slimes");
}

namespace BSConsoleVariables
{
	// =================================================================================================================
	// User Settings CVars
	// =================================================================================================================
	
	TAutoConsoleVariable<float> CVarBSLookSensitivityX(
	TEXT("BS.UserSettings.MouseSensitivity.X"),
	1,
	TEXT("1 = Default"),
	ECVF_Default);

	TAutoConsoleVariable<float> CVarBSLookSensitivityY(
	TEXT("BS.UserSettings.MouseSensitivity.Y"),
	1,
	TEXT("1 = Default"),
	ECVF_Default);

	TAutoConsoleVariable<float> CVarBSCameraOffset(
	TEXT("BS.UserSettings.CameraOffset"),
	1,
	TEXT("1 = Default"),
	ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarBSBloomEnabled(
	TEXT("BS.UserSettings.BloomEnabled"),
	true,
	TEXT("true = Default"),
	ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarBSDebugHitDetection(
	TEXT("BS.Debug.HitDetect.Enable"),
	false,
	TEXT("false = Default"),
	ECVF_Default);
	
	TAutoConsoleVariable<float> CVarBSDebugHitDetectionDuration(
	TEXT("BS.Debug.HitDetect.Duration"),
	1,
	TEXT("1 = Default"),
	ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarBSDebugAggroSpheres(
	TEXT("BS.Debug.AggroSphere.Enable"),
	false,
	TEXT("false = Default"),
	ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarBSBarsShowNumbers(
	TEXT("BS.NamePlates.ShowNumbers"),
	false,
	TEXT("false = Default"),
	ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarBSBarsShowPercentages(
	TEXT("BS.NamePlates.ShowPercentages"),
	false,
	TEXT("false = Default"),
	ECVF_Default);	
	
	TAutoConsoleVariable<bool> CVarShowPlayerHoverNames(
	TEXT("BS.PlayerNames.ShowOthers"),
	true,
	TEXT("true = Default"),
	ECVF_Default);	
	
	TAutoConsoleVariable<bool> CVarShowOwnHoverName(
	TEXT("BS.PlayerNames.ShowSelf"),
	false,
	TEXT("false = Default"),
	ECVF_Default);
}