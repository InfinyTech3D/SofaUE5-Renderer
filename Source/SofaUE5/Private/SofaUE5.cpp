// Copyright Epic Games, Inc. All Rights Reserved.

#include "SofaUE5.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "SofaUE5Library/ExampleLibrary.h"
//#include "SofaUE5Library/SofaPhysicsAPI.h"
#include "SofaUE5Library/SofaPhysicsBindings.h"

#define LOCTEXT_NAMESPACE "FSofaUE5Module"

//DEFINE_LOG_CATEGORY(YourLog);

void FSofaUE5Module::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("SofaUE5")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	//LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/SofaUE5Library/Win64/ExampleLibrary.dll"));
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/SofaUE5Library/Win64/SofaPhysicsAPI.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SofaUE5Library/Mac/Release/libSofaPhysicsAPI.dylib"));
#elif PLATFORM_LINUX
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/SofaUE5Library/Linux/x86_64-unknown-linux-gnu/libSofaPhysicsAPI.so"));
#endif // PLATFORM_WINDOWS

	//FPlatformProcess::AddDllDirectory(*FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SofaUnrealLibrary/sofa/bin/")));
	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	UE_LOG(SUnreal_log, Warning, TEXT("######### FSofaUE5Module::StartupModule() ##########"));
	if (ExampleLibraryHandle)
	{
		// Call the test function in the third party library that opens a message box
		//float res = FooPluginFunction();
		//ExampleLibraryFunction();
		//int test = test_getAPI_ID();
		UE_LOG(SUnreal_log, Warning, TEXT("######### FSofaUE5Module::StartupModule() start: ##########"));
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "SUCCESS to load SofaPhysicsAPI library"));
	}
	else
	{
		if (LibraryPath.IsEmpty())
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load SofaPhysicsAPI library path empty"));
		else
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load SofaPhysicsAPI GetDllHandle error"));
	}
}

void FSofaUE5Module::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	ExampleLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSofaUE5Module, SofaUE5)
