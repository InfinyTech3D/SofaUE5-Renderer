// Fill out your copyright notice in the Description page of Project Settings.

#include "SofaVisualMesh.h"
#include "SofaUnreal.h"


// Sets default values
ASofaVisualMesh::ASofaVisualMesh()
    : m_impl(NULL)
    , isStatic(false)
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
    createMesh();
    SetActorScale3D(FVector(10.0, 10.0, 10.0));
}

// Called when the game starts or when spawned
void ASofaVisualMesh::BeginPlay()
{
    Super::BeginPlay();
}

// This is called when actor is spawned (at runtime or when you drop it into the world in editor)
void ASofaVisualMesh::PostActorCreated()
{
    Super::PostActorCreated();
    //createMesh();
}

// This is called when actor is already in level and map is opened
void ASofaVisualMesh::PostLoad()
{
    Super::PostLoad();
    //createMesh();
}

// Called every frame
void ASofaVisualMesh::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );

    if (!isStatic)
        updateMesh();


    /*
    FVector NewLocation = GetActorLocation();
    float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
    NewLocation.Z += DeltaHeight * m_scaleOsci;       //Scale our height by a factor of 20
    RunningTime += DeltaTime;
    SetActorLocation(NewLocation);*/
}


void ASofaVisualMesh::updateMesh()
{
    if (m_impl == NULL)
        return;

    int nbrV = m_impl->getNbVertices();
    float* sofaVertices = new float[nbrV * 3];
    float* sofaNormals = new float[nbrV * 3];

    // Get the different buffers
    m_impl->getVPositions(sofaVertices);
    m_impl->getVNormals(sofaNormals);

    TArray<FVector> vertices;
    TArray<FVector> normals;
    for (int i = 0; i < nbrV; i++)
    {
        vertices.Add(FVector(sofaVertices[i * 3], sofaVertices[i * 3 + 1], sofaVertices[i * 3 + 2]));
        normals.Add(FVector(sofaNormals[i * 3], sofaNormals[i * 3 + 1], sofaNormals[i * 3 + 2]));
    }
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(nbrV));
    mesh->UpdateMeshSection(0, vertices, normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>());
}

//DEFINE_LOG_CATEGORY(YourLog);
void ASofaVisualMesh::createMesh()
{
    if (m_impl == NULL)
        return;

    FString type2 = m_impl->getObjectType().c_str();
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, type2);


    // Get topology elements numbers
    int nbrV = m_impl->getNbVertices();
    int nbrTri = m_impl->getNbTriangles();
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(nbrV));
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(nbrTri));

    float* sofaVertices = new float[nbrV*3];
    float* sofaNormals = new float[nbrV*3];
    float* sofaTexCoords = new float[nbrV * 2];
    int* sofaTriangles = new int[nbrTri*3];

    // Get the different buffers
    m_impl->getVPositions(sofaVertices);
    m_impl->getVNormals(sofaNormals);
    m_impl->getTriangles(sofaTriangles);
    m_impl->getVTexCoords(sofaTexCoords);

    // Convert in Unreal structure

    TArray<FVector> vertices;
    TArray<int32> Triangles;
    TArray<FVector> normals;
    TArray<FVector2D> UV0;
    TArray<FProcMeshTangent> tangents;
    TArray<FLinearColor> vertexColors;

    for (int i = 0; i < nbrV; i++)
    {
        vertices.Add(FVector(sofaVertices[i * 3], sofaVertices[i * 3 + 1], sofaVertices[i * 3 + 2]));
        normals.Add(FVector(sofaNormals[i * 3], sofaNormals[i * 3 + 1], sofaNormals[i * 3 + 2]));

        UV0.Add(FVector2D(sofaTexCoords[i * 3], sofaTexCoords[i * 3 + 1]));
        tangents.Add(FProcMeshTangent(0, 1, 0));
        vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
    }

    for (int i = 0; i < nbrTri; i++)
    {
        Triangles.Add(sofaTriangles[i * 3]);
        Triangles.Add(sofaTriangles[i * 3 + 1]);
        Triangles.Add(sofaTriangles[i * 3 + 2]);
    }

    mesh->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);

    // Enable collision data
    mesh->ContainsPhysicsTriMeshData(true);
}
