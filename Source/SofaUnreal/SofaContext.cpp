// Fill out your copyright notice in the Description page of Project Settings.

#include "SofaUnreal.h"
#include "SofaContext.h"

/*
void *v_sofaDLLHandle;

#pragma region Load DLL
// Method to import a DLL.
bool ASofaContext::importDLL(FString folder, FString name)
{
	FString filePath = *FPaths::GamePluginsDir() + folder + "/" + name;

	if (FPaths::FileExists(filePath))
	{
		v_sofaDLLHandle = FPlatformProcess::GetDllHandle(*filePath); // Retrieve the DLL.
		if (v_sofaDLLHandle != NULL)
		{
			return true;
		}
	}
	return false;	// Return an error.
}
#pragma endregion Load DLL

#pragma region Unload DLL
// If you love something  set it free.
void ASofaContext::freeDLL()
{
	if (v_sofaDLLHandle != NULL)
	{
		//m_FooPluginFunctionDll = NULL;

		FPlatformProcess::FreeDllHandle(v_sofaDLLHandle);
		v_sofaDLLHandle = NULL;
	}
}
#pragma endregion Unload DLL
*/

// Sets default values
ASofaContext::ASofaContext()
	: m_dllLoadStatus(0)
	, m_apiName("")
	, m_sofaAPI(NULL)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

// Called when the game starts or when spawned
void ASofaContext::BeginPlay()
{
	Super::BeginPlay();
	
	// create a new sofa context through sofaAdvancePhysicsAPI
	m_sofaAPI = new SofaAdvancePhysicsAPI();
	m_apiName = m_sofaAPI->APIName();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, m_apiName);

	// Create the scene.
	m_sofaAPI->createScene();

	FString toto = m_sofaAPI->getTestName().c_str();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, toto);
	m_sofaAPI->setTestName("TESTTRUC");
	
	// load scene
	if (!filePath.FilePath.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, filePath.FilePath);
		const char* scenePath = "C:/Users/Belcurves/projects/Unreal Projects/SofaUnreal/Content/SofaScenes/TriangleSurfaceCutting.scn"; //TCHAR_TO_ANSI(*filePath.FilePath);

		

		bool resScene = false; //m_sofaAPI->load(scenePath);
		
		if (resScene)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "scene ok");
		else
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "scene failed");	
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, filePath.FilePath);
	
	toto = m_sofaAPI->getTestName().c_str();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, toto);

	// Import the SofaPAPI dll
	//bool res = importDLL("SofaDLLs", "SofaAdvancePhysicsAPI.dll");

	//if (res) {

	//}
}

// Called every frame
void ASofaContext::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	//toto = "Truc2";// FString::SanitizeFloat(resMethod);
	
}

