// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealCombatAIDemo : ModuleRules
{
	public UnrealCombatAIDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
            "NavigationSystem",
            "GameplayTasks",
            "StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"UnrealCombatAIDemo",
			"UnrealCombatAIDemo/Variant_Platforming",
			"UnrealCombatAIDemo/Variant_Platforming/Animation",
			"UnrealCombatAIDemo/Variant_Combat",
			"UnrealCombatAIDemo/Variant_Combat/AI",
			"UnrealCombatAIDemo/Variant_Combat/Animation",
			"UnrealCombatAIDemo/Variant_Combat/Gameplay",
			"UnrealCombatAIDemo/Variant_Combat/Interfaces",
			"UnrealCombatAIDemo/Variant_Combat/UI",
			"UnrealCombatAIDemo/Variant_SideScrolling",
			"UnrealCombatAIDemo/Variant_SideScrolling/AI",
			"UnrealCombatAIDemo/Variant_SideScrolling/Gameplay",
			"UnrealCombatAIDemo/Variant_SideScrolling/Interfaces",
			"UnrealCombatAIDemo/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
