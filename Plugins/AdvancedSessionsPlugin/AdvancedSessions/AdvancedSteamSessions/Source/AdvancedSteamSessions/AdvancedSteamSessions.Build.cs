using UnrealBuildTool;
using System.IO;
 
public class AdvancedSteamSessions : ModuleRules
{
    public AdvancedSteamSessions(ReadOnlyTargetRules Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        //bEnforceIWYU = true;
        
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore", 
            "OnlineSubsystem", 
            "CoreUObject", 
            "OnlineSubsystemUtils", 
            "Networking", 
            "Sockets",
            "AdvancedSessions",
        });
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "Sockets", "Networking", "OnlineSubsystemUtils"});

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Linux) || (Target.Platform == UnrealTargetPlatform.Mac))
        {
            PublicDependencyModuleNames.AddRange(new string[] { "SteamShared", "Steamworks", "OnlineSubsystemSteam", "SteamSockets" });
            AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");
            //PublicIncludePaths.AddRange(new string[] { "../Plugins/Online/OnlineSubsystemSteam/Source/Private" });// This is dumb but it isn't very open
        }
    }
}