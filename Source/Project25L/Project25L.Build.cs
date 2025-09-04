// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project25L : ModuleRules
{
	public Project25L(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
		PublicIncludePaths.AddRange(new string[] { "Project25L" });

        PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"ModularGameplay",
			"GameplayTags",
			"NetCore",
			"UMG",
            "Niagara",
            "LevelSequence",
            "MovieScene",
			"MovieSceneTracks",
            "NavigationSystem",
            "AIModule",
			"Slate",
			"SlateCore",
            "AnimGraphRuntime",
        });

		
		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        if (Target.bBuildEditor) // 에디터 빌드일 때만
        {
            PrivateDependencyModuleNames.AddRange(new string[] {
                "UnrealEd"
            });
        }
    }
}
