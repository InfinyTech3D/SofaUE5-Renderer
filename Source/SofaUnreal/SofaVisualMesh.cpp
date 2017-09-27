// Fill out your copyright notice in the Description page of Project Settings.

#include "SofaUnreal.h"
#include "SofaVisualMesh.h"


// Sets default values
ASofaVisualMesh::ASofaVisualMesh()
	: m_impl(NULL)
{

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_scaleOsci = 20.0f;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
}

void ASofaVisualMesh::setSofaImpl(Sofa3DObject * impl)
{
	m_impl = impl;
}

// Called when the game starts or when spawned
void ASofaVisualMesh::BeginPlay()
{
	Super::BeginPlay();

	if (m_impl != NULL)
	{
		FString type2 = m_impl->getObjectType().c_str();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, type2);
	}

	
}

// This is called when actor is spawned (at runtime or when you drop it into the world in editor)
void ASofaVisualMesh::PostActorCreated()
{
	Super::PostActorCreated();
	CreateTriangle();
}

// This is called when actor is already in level and map is opened
void ASofaVisualMesh::PostLoad()
{
	Super::PostLoad();
	CreateTriangle();
}

// Called every frame
void ASofaVisualMesh::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	FVector NewLocation = GetActorLocation();
	float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
	NewLocation.Z += DeltaHeight * m_scaleOsci;       //Scale our height by a factor of 20
	RunningTime += DeltaTime;
	SetActorLocation(NewLocation);
}

void ASofaVisualMesh::CreateTriangle()
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

