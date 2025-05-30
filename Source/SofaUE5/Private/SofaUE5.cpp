/*****************************************************************************
 *                 - Copyright (C) - 2022 - InfinyTech3D -                   *
 *                                                                           *
 * This file is part of the SofaUE5-Renderer asset from InfinyTech3D         *
 *                                                                           *
 * GNU General Public License Usage:                                         *
 * This file may be used under the terms of the GNU General                  *
 * Public License version 3. The licenses are as published by the Free       *
 * Software Foundation and appearing in the file LICENSE.GPL3 included in    *
 * the packaging of this file. Please review the following information to    *
 * ensure the GNU General Public License requirements will be met:           *
 * https://www.gnu.org/licenses/gpl-3.0.html.                                *
 *                                                                           *
 * Commercial License Usage:                                                 *
 * Licensees holding valid commercial license from InfinyTech3D may use this *
 * file in accordance with the commercial license agreement provided with    *
 * the Software or, alternatively, in accordance with the terms contained in *
 * a written agreement between you and InfinyTech3D. For further information *
 * on the licensing terms and conditions, contact: contact@infinytech3d.com  *
 *                                                                           *
 * Authors: see Authors.txt                                                  *
 * Further information: https://infinytech3d.com                             *
 ****************************************************************************/

#include "SofaUE5.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "SofaUE5Library/SofaPhysicsAPI.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#define LOCTEXT_NAMESPACE "FSofaUE5Module"
DEFINE_LOG_CATEGORY(SUnreal_log);
DEFINE_LOG_CATEGORY(SofaLog);


void FSofaUE5Module::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("SofaUE5")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/SofaUE5Library/Win64/SofaPhysicsAPI.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SofaUE5Library/Mac/Release/libSofaPhysicsAPI.dylib"));
#elif PLATFORM_LINUX
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/SofaUE5Library/Linux/x86_64-unknown-linux-gnu/libSofaPhysicsAPI.so"));
#endif // PLATFORM_WINDOWS



	//FPlatformProcess::AddDllDirectory(*FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SofaUnrealLibrary/sofa/bin/")));
	if (LibraryPath.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load SofaPhysicsAPI.dll, library path is empty!"));
		return;
	}
	else if (!FPaths::FileExists(LibraryPath))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load SofaPhysicsAPI.dll, File not found at given path"));
		return;
	}

	FPlatformProcess::AddDllDirectory(*FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/SofaUE5Library/Win64/")));
	ExampleLibraryHandle = FPlatformProcess::GetDllHandle(*LibraryPath);
	
	if (ExampleLibraryHandle)
	{
		// Call the test function in the third party library that opens a message box
		//float res = FooPluginFunction();
		//ExampleLibraryFunction();
		//int test = test_getAPI_ID();
		//UE_LOG(SUnreal_log, Warning, TEXT("######### FSofaUE5Module::StartupModule() start: ##########"));
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "SUCCESS to load SofaPhysicsAPI library"));
	}
	else
	{
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
