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
	m_scaleOsci = 20.0f;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
	/**
	*	Create/replace a section for this procedural mesh component.
	*	@param	SectionIndex		Index of the section to create or replace.
	*	@param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
	*	@param	Triangles			Index buffer indicating which vertices make up each triangle. Length must be a multiple of 3.
	*	@param	Normals				Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	*	@param	UV0					Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	*	@param	VertexColors		Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	*	@param	Tangents			Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	*	@param	bCreateCollision	Indicates whether collision should be created for this section. This adds significant cost.
	*/
	//UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh", meta = (AutoCreateRefTerm = "Normals,UV0,VertexColors,Tangents"))
	//	void CreateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals,
	// const TArray<FVector2D>& UV0, const TArray<FColor>& VertexColors, const TArray<FProcMeshTangent>& Tangents, bool bCreateCollision);

	// New in UE 4.17, multi-threaded PhysX cooking.
	//mesh->bUseAsyncCooking = true;

}

// This is called when actor is spawned (at runtime or when you drop it into the world in editor)
void ACubeFEM::PostActorCreated()
{
	Super::PostActorCreated();
	CreateTriangle();
}

// This is called when actor is already in level and map is opened
void ACubeFEM::PostLoad()
{
	Super::PostLoad();
	CreateTriangle();
}

void ACubeFEM::CreateTriangle()
{
	TArray<FVector> vertices;
	vertices.Add(FVector(0, 0, 0));
	vertices.Add(FVector(0, 100, 0));
	vertices.Add(FVector(0, 0, 100));

	TArray<int32> Triangles;
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);

	TArray<FVector> normals;
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));

	TArray<FVector2D> UV0;
	UV0.Add(FVector2D(0, 0));
	UV0.Add(FVector2D(10, 0));
	UV0.Add(FVector2D(0, 10));


	TArray<FProcMeshTangent> tangents;
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));

	TArray<FLinearColor> vertexColors;
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));

	mesh->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);

	// Enable collision data
	mesh->ContainsPhysicsTriMeshData(true);
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
	NewLocation.Z += DeltaHeight * m_scaleOsci;       //Scale our height by a factor of 20
	RunningTime += DeltaTime;
	SetActorLocation(NewLocation);		
	
	FString toto = FString::SanitizeFloat(resMethod);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, toto);
}


//EXPORT_API float FooPluginFunction() { return 5.f; }
