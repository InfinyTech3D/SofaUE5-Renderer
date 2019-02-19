// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include <iomanip>
#include <string>
#include <sstream>

DECLARE_LOG_CATEGORY_EXTERN(SUnreal_log, Log, All);
DEFINE_LOG_CATEGORY(SUnreal_log);

DECLARE_LOG_CATEGORY_EXTERN(SofaLog, Log, All);
DEFINE_LOG_CATEGORY(SofaLog);
//DECLARE_LOG_CATEGORY_EXTERN(YourLog, Log, All);

static FString intToHexa(int value)
{
    std::stringstream stream;
    stream << std::hex << value;
    return FString(stream.str().c_str());
}

class FSofaUnrealModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Handle to the test dll we will load */
	void*	ExampleLibraryHandle;
};
