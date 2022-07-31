// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UnrealEnginePAG : ModuleRules
{
	private string ThirdPartyDirectory => Path.Combine(ModuleDirectory, "../ThirdParty/");

	private string LibsDirectory => ThirdPartyDirectory + "libs/";
	
	public UnrealEnginePAG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"RHI",
				"RenderCore",
				"UMG",
			});
		
		PrivateIncludePaths.Add(ThirdPartyDirectory + "include");
		
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add(LibsDirectory + "pag.lib");

			// string LibEGLDll = "libEGL.dll";
			// string LibGLESv2 = "libGLESv2.dll";
			
			// PublicDelayLoadDLLs.Add(LibEGLDll);
			// PublicDelayLoadDLLs.Add(LibGLESv2);
			
			RuntimeDependencies.Add("$(TargetOutputDir)/libEGL.dll", Path.Combine(PluginDirectory, "Binaries", "Win64", "libEGL.dll"));
			RuntimeDependencies.Add("$(TargetOutputDir)/libGLESv2.dll", Path.Combine(PluginDirectory, "Binaries", "Win64", "libGLESv2.dll"));
			
			// AddEngineThirdPartyPrivateStaticDependencies(Target, "FreeType2");
			// PublicAdditionalLibraries.Add(LibsDirectory + "libEGL.lib");
			// PublicAdditionalLibraries.Add(LibsDirectory + "libGLESv2.lib");
		}
		else if (Target.Platform == UnrealTargetPlatform.Android)
		{
			
		}
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			
		}
	}
}
