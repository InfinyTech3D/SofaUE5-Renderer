// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SofaUnreal.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "SofaUnrealLibrary/ExampleLibrary.h"
#include <SofaAdvancePhysicsAPI/SofaPhysicsBindings.h>

//DEFINE_LOG_CATEGORY(YourLog);

#define LOCTEXT_NAMESPACE "FSofaUnrealModule"

void FSofaUnrealModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("SofaUnreal")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SofaUnrealLibrary/sofa/bin/SofaAdvancePhysicsAPI.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SofaUnrealLibrary/Mac/Release/libExampleLibrary.dylib"));
#endif // PLATFORM_WINDOWS

    FPlatformProcess::AddDllDirectory(*FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SofaUnrealLibrary/sofa/bin/")));
	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (ExampleLibraryHandle)
	{
        //FPlatformProcess::GetDllHandle(*FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SofaUnrealLibrary/sofa/bin/SceneCreator.dll")));
		// Call the test function in the third party library that opens a message box
        float res = FooPluginFunction();
        UE_LOG(SUnreal_log, Warning, TEXT("TEST DLL: FooPluginFunction return res: %f"), res);
		//ExampleLibraryFunction();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}
}

void FSofaUnrealModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	ExampleLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSofaUnrealModule, SofaUnreal)
