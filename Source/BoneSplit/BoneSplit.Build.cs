// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BoneSplit : ModuleRules
{
	public BoneSplit(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
		[
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			"AIModule",
			"CommonUI",
			"UMG",
			"Slate",
			"SlateCore",
			"Niagara",
			"NavigationSystem",
			"DeveloperSettings",
			"AnimGraphRuntime",
			"ClientAuthoritativeCharacterSystem",
			"CustomThumbnails",
			"PhysicsCore",
			"Chooser", 
			"AdvancedSessions",
			"AdvancedSteamSessions",
			"OnlineSubsystemUtils",
			"Networking",
			"OnlineSubsystem",
			"ControlRig",
			"RigVM", 
			"CharacterAbilities",
			"ShapeOverlapLibrary",
			"Mobs",
			"AdvancedProgressBar"
		]);
	}
}
