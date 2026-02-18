// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TP1Reseau : ModuleRules
{
	public TP1Reseau(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"TP1Reseau",
			"TP1Reseau/Variant_Platforming",
			"TP1Reseau/Variant_Platforming/Animation",
			"TP1Reseau/Variant_Combat",
			"TP1Reseau/Variant_Combat/AI",
			"TP1Reseau/Variant_Combat/Animation",
			"TP1Reseau/Variant_Combat/Gameplay",
			"TP1Reseau/Variant_Combat/Interfaces",
			"TP1Reseau/Variant_Combat/UI",
			"TP1Reseau/Variant_SideScrolling",
			"TP1Reseau/Variant_SideScrolling/AI",
			"TP1Reseau/Variant_SideScrolling/Gameplay",
			"TP1Reseau/Variant_SideScrolling/Interfaces",
			"TP1Reseau/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
