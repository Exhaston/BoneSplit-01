// Some copyright should be here...

using UnrealBuildTool;

public class GameplayAbilitiesExtension : ModuleRules
{
	public GameplayAbilitiesExtension(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange([
			"Core",
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags", 
			"ModularGameplay", "EnhancedInput"
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
