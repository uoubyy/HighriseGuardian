// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectFPS : ModuleRules
{
	public ProjectFPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",
            "UMG", "Niagara", "HTTP", "SlateCore", "OnlineSubsystem", "OnlineSubsystemUtils", "Steamworks" });

		PrivateDependencyModuleNames.AddRange(new string[] { "NakamaUnreal", "NakamaCore", "Json", "JsonUtilities" });

        OptimizeCode = CodeOptimization.Never;

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        PrivateDependencyModuleNames.Add("OnlineSubsystemSteam");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
