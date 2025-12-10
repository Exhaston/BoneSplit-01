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
	// Equipment Tags
	// =================================================================================================================

	UE_DEFINE_GAMEPLAY_TAG(Equipment, "Equipment");

	UE_DEFINE_GAMEPLAY_TAG(Equipment_Part, "Equipment.Part");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Part_Head, "Equipment.Part.Head");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Part_Chest, "Equipment.Part.Chest");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Part_Arms, "Equipment.Part.Arms");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Part_Legs, "Equipment.Part.Legs");

	UE_DEFINE_GAMEPLAY_TAG(Equipment_Misc, "Equipment.Misc");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Misc_Curio, "Equipment.Misc.Curio");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Misc_Trinket, "Equipment.Misc.Trinket");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Misc_Soul, "Equipment.Misc.Soul");

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
	ECVF_Default
	);

	TAutoConsoleVariable<float> CVarBSLookSensitivityY(
	TEXT("BS.UserSettings.MouseSensitivity.Y"),
	1,
	TEXT("1 = Default"),
	ECVF_Default
	);

	TAutoConsoleVariable<float> CVarBSCameraOffset(
	TEXT("BS.UserSettings.CameraOffset"),
	1,
	TEXT("1 = Default"),
	ECVF_Default
	);
	
	TAutoConsoleVariable<bool> CVarBSBloomEnabled(
	TEXT("BS.UserSettings.BloomEnabled"),
	true,
	TEXT("true = Default"),
	ECVF_Default
	);
}