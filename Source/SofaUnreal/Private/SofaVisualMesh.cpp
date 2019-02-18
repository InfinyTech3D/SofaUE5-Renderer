// Fill out your copyright notice in the Description page of Project Settings.

#include "SofaVisualMesh.h"
#include "SofaUnreal.h"

//DEFINE_LOG_CATEGORY(SUnreal_log);

// Sets default values
ASofaVisualMesh::ASofaVisualMesh()
    : m_impl(NULL)
    , isStatic(false)
    , m_min(FVector(100000, 100000, 100000))
    , m_max(FVector(-100000, -100000, -100000))
{
    UE_LOG(SUnreal_log, Warning, TEXT("##### ASofaVisualMesh::ASofaVisualMesh() ####"));
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
    RootComponent = mesh;
}

void ASofaVisualMesh::setSofaImpl(Sofa3DObject * impl)
{
    m_impl = impl;
    createMesh();
    //SetActorScale3D(FVector(10.0, 10.0, 10.0));
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
}


void ASofaVisualMesh::updateMesh()
{    
    if (m_impl == NULL)
        return;

    int nbrV = m_impl->getNbVertices();
    //UE_LOG(SUnreal_log, Warning, TEXT("ASofaVisualMesh::updateMesh(): %d"), nbrV);
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

//DEFINE_LOG_CATEGORY(SUnreal_log);
void ASofaVisualMesh::createMesh()
{
    if (m_impl == NULL)
        return;

    FString type2 = m_impl->getObjectType().c_str();
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, type2);
    
    // Get topology elements numbers
    int nbrV = m_impl->getNbVertices();
    int nbrTri = m_impl->getNbTriangles();
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaVisualMesh::createMesh(): nbrV: %d | nbrTri: %d"), nbrV, nbrTri);

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
        UV0.Add(FVector2D(sofaTexCoords[i * 2], sofaTexCoords[i * 2 + 1]));

        tangents.Add(FProcMeshTangent(0, 1, 0));
        vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
    }

    //recomputeUV(vertices, UV0);


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

void ASofaVisualMesh::computeBoundingBox(const TArray<FVector>& vertices)
{
    int nbrV = vertices.Num();

    // Get min and max of the mesh
    for (int i = 0; i < nbrV; i++)
    {
        const FVector& vertex = vertices[i];        
        if (vertex.X > m_max.X)
            m_max.X = vertex.X;
        if (vertex.Y > m_max.Y)
            m_max.Y = vertex.Y;
        if (vertex.Z > m_max.Z)
            m_max.Z = vertex.Z;

        if (vertex.X < m_min.X)
            m_min.X = vertex.X;
        if (vertex.Y < m_min.Y)
            m_min.Y = vertex.Y;
        if (vertex.Z < m_min.Z)
            m_min.Z = vertex.Z;
    }
}

void ASofaVisualMesh::recomputeUV(const TArray<FVector>& vertices, TArray<FVector2D>& UV0)
{  
    int nbrV = vertices.Num();

    computeBoundingBox(vertices);
    FVector center = (m_max + m_min)*0.5f;

    // Map mesh vertices to sphere
    // transform cart to spherical coordinates (r, theta, phi) and sphere to cart back with radius = 1
    TArray<FVector> sphereV;
    sphereV.SetNum(nbrV);
    UV0.SetNum(nbrV);
    
    for (int i = 0; i < nbrV; ++i)
    {
        FVector Vcentered = vertices[i] - center;
        float r = sqrtf(Vcentered.X*Vcentered.X + Vcentered.Y*Vcentered.Y + Vcentered.Z*Vcentered.Z);
        float theta = acos(Vcentered.Z / r);
        float phi = atan(Vcentered.Y / Vcentered.X);

        sphereV[i].X = sin(phi)*cos(theta);
        sphereV[i].Y = sin(phi)*sin(theta);
        sphereV[i].Z = cos(phi);
    }

    // transform sphere coordinates to UV map
    for (int i = 0; i < nbrV; ++i)
    {
        FVector pos = (sphereV[i] - center);
        pos.Normalize();
        UV0[i].X = 0.5 + atan2(pos.Z, pos.X) / (2 * PI);
        UV0[i].Y = 0.5 - asin(pos.Y) / PI;
    }

}
