// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class SofaUE5Library : ModuleRules
{
	public SofaUE5Library(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
            //PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

            string SofaLibPath;
            string SofaBinName;
            string SofaBinPath;

            // Debug mode
            if (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.DebugGame)
            {
                SofaLibPath = Path.Combine(ModuleDirectory, "x64", "Debug", "SofaVerseAPI_d.lib");
                SofaBinName = "SofaVerseAPI_d.dll";
                SofaBinPath = Path.Combine("$(PluginDir)/Binaries/ThirdParty/SofaUE5Library/Win64/", "Debug", SofaBinName);

                PublicDefinitions.Add("WITH_SOFA_DEBUG=1");
            }
            else
            {
                SofaLibPath = Path.Combine(ModuleDirectory, "x64", "Release", "SofaVerseAPI.lib");
                SofaBinName = "SofaVerseAPI.dll";
                SofaBinPath = Path.Combine("$(PluginDir)/Binaries/ThirdParty/SofaUE5Library/Win64/", "Release", SofaBinName);
            }

            // Add the import library
            PublicAdditionalLibraries.Add(SofaLibPath);

            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add(SofaBinName);

            // Ensure that the DLL is staged along with the executable
            RuntimeDependencies.Add(SofaBinPath);
            //RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/SofaUE5Library/Win64/SofaPhysicsAPI.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            //PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libExampleLibrary.dylib"));
            //RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/SofaUE5Library/Mac/Release/libExampleLibrary.dylib");
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			//string ExampleSoPath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "SofaUE5Library", "Linux", "x86_64-unknown-linux-gnu", "libExampleLibrary.so");
			//PublicAdditionalLibraries.Add(ExampleSoPath);
			//PublicDelayLoadDLLs.Add(ExampleSoPath);
			//RuntimeDependencies.Add(ExampleSoPath);
		}
	}
}
