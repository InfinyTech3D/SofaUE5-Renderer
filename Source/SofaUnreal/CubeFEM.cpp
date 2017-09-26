// Fill out your copyright notice in the Description page of Project Settings.

#include "SofaUnreal.h"
#include "CubeFEM.h"

typedef float(*_FooPluginFunction)(); // Declare a method to store the DLL method getInvertedBool.
_FooPluginFunction m_FooPluginFunctionDll;

void *v_dllHandle;

#pragma region Load DLL

// Method to import a DLL.
bool ACubeFEM::importDLL(FString folder, FString name)
{
	FString filePath = *FPaths::GamePluginsDir() + folder + "/" + name;

	if (FPaths::FileExists(filePath))
	{
		v_dllHandle = FPlatformProcess::GetDllHandle(*filePath); // Retrieve the DLL.
		if (v_dllHandle != NULL)
		{
			return true;
		}
	}
	return false;	// Return an error.
}
#pragma endregion Load DLL


#pragma region Import Methods

// Imports the method getInvertedBool from the DLL.
bool ACubeFEM::importMethodFooPluginFunction()
{
	if (v_dllHandle != NULL)
	{
		m_FooPluginFunctionDll = NULL;
		FString procName = "FooPluginFunction";	// Needs to be the exact name of the DLL method.
		m_FooPluginFunctionDll = (_FooPluginFunction)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_FooPluginFunctionDll != NULL)
		{
			return true;
		}
	}
	return false;	// Return an error.
}

#pragma endregion Import Methods


#pragma region Method Calls

// Calls the method getInvertedBoolFromDll that was imported from the DLL.
float ACubeFEM::FooPluginFunctionFromDLL()
{
	if (m_FooPluginFunctionDll != NULL)
	{
		float out = float(m_FooPluginFunctionDll()); // Call the DLL method with arguments corresponding to the exact signature and return type of the method.
		return out;
	}
	return -99.0;	// Return an error.
}

#pragma endregion Method Calls



#pragma region Unload DLL

// If you love something  set it free.
void ACubeFEM::freeDLL()
{
	if (v_dllHandle != NULL)
	{
		m_FooPluginFunctionDll = NULL;

		FPlatformProcess::FreeDllHandle(v_dllHandle);
		v_dllHandle = NULL;
	}
}
#pragma endregion Unload DLL


// Sets default values
ACubeFEM::ACubeFEM()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	resMethod = 10.f;
}

// Called when the game starts or when spawned
void ACubeFEM::BeginPlay()
{
	bool res = importDLL("SofaDLLs", "SofaAdvancePhysicsAPI.dll");
	//bool res = importDLL("SofaDLLs", "SofaPhysicsAPI.dll");
	//res = importDLL("SofaDLLs", "SofaGuiMain.dll");
	if (res) {
		res = importMethodFooPluginFunction();

		if (res)
			resMethod = FooPluginFunctionFromDLL();
		else
			resMethod = -20.0f;
	}
	else
		resMethod = -10.0f;
	
	Super::BeginPlay();	

}

//void ACubeFEM::EndPlay()
//{
//	freeDLL();
//	Super::EndPlay();
//}

// Called every frame
void ACubeFEM::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation();
	float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
	NewLocation.Z += DeltaHeight * 20.0f;       //Scale our height by a factor of 20
	RunningTime += DeltaTime;
	SetActorLocation(NewLocation);		
	
	FString toto = FString::SanitizeFloat(resMethod);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, toto);
}


//EXPORT_API float FooPluginFunction() { return 5.f; }
