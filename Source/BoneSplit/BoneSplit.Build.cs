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
			"Niagara",
			"NavigationSystem",
			"DeveloperSettings",
			"AnimGraphRuntime",
			"ClientAuthoritativeCharacterSystem",
			"CustomThumbnails",
			"PhysicsCore"
		]);
	}
}
