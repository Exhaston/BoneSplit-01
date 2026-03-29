// Some copyright should be here...

using UnrealBuildTool;

public class CharacterAbilities : ModuleRules
{
	public CharacterAbilities(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange([
			"Core",
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			"EnhancedInput"
		]);
		
		PrivateDependencyModuleNames.AddRange([
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore"
		]);
		
		DynamicallyLoadedModuleNames.AddRange([
			
		]);
	}
}
