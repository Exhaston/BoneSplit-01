// Copyright 2023 Anton Hesselbom. All Rights Reserved.

using UnrealBuildTool;

public class ClientAuthoritativeCharacterSystem : ModuleRules
{
	public ClientAuthoritativeCharacterSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "Engine", "GameplayTasks", "GameplayAbilities" });
		PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine", "NetCore" });
	}
}
