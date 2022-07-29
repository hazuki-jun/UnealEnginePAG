// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealEnginePAGEditor : ModuleRules
{
	public UnrealEnginePAGEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UnrealEnginePAG"
				// ... add other public dependencies that you statically link with here ...
			});
		
		PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
	}
}
