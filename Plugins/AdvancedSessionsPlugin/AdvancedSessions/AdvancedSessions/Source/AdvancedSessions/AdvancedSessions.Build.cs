using UnrealBuildTool;
using System.IO;
 
public class AdvancedSessions : ModuleRules
{
    public AdvancedSessions(ReadOnlyTargetRules Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        //bEnforceIWYU = true;
        
        PublicDefinitions.Add("WITH_ADVANCED_SESSIONS=1");
        
       PrivateIncludePaths.AddRange([Path.Combine(ModuleDirectory, "Private")]);
       PublicIncludePaths.AddRange([Path.Combine(ModuleDirectory, "Public")]);
       
       PublicDependencyModuleNames.AddRange([
	       "Core", 
	       "CoreUObject", 
	       "Engine", 
	       "InputCore", 
	       "OnlineSubsystem", 
	       "CoreUObject", 
	       "OnlineSubsystemUtils", 
	       "Networking", 
	       "Sockets", 
	       "OnlineSubsystemSteam"
       ]);
        
    }
}