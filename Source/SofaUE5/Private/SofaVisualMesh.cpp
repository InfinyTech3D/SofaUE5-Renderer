// Fill out your copyright notice in the Description page of Project Settings.

#include "SofaVisualMesh.h"
#include "SofaUE5.h"
#include "SofaUE5Library/SofaPhysicsAPI.h"

// Sets default values
ASofaVisualMesh::ASofaVisualMesh()
    : m_isStatic(false)
    , m_min(FVector(100000, 100000, 100000))
    , m_max(FVector(-100000, -100000, -100000))
{
    UE_LOG(SUnreal_log, Warning, TEXT("##### ASofaVisualMesh::ASofaVisualMesh() ####"));
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
    RootComponent = mesh;
}

void ASofaVisualMesh::setSofaMesh(SofaPhysicsOutputMesh* sofaMesh)
{
    m_sofaMesh = sofaMesh;
    createMesh();
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
}

// This is called when actor is already in level and map is opened
void ASofaVisualMesh::PostLoad()
{
    Super::PostLoad();
}

// Called every frame
void ASofaVisualMesh::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );

    if (!m_isStatic)
        updateMesh();
}


void ASofaVisualMesh::updateMesh()
{   
    if (m_sofaMesh == nullptr)
        return;

    // Get number of vertices
    int nbrV = m_sofaMesh->getNbVertices();

    // Get the different buffers
    float* sofaVertices = new float[nbrV * 3];
    float* sofaNormals = new float[nbrV * 3];
    m_sofaMesh->getVPositions(sofaVertices);
    m_sofaMesh->getVNormals(sofaNormals);

    // Copy data into UE structure
    TArray<FVector> vertices;
    TArray<FVector> normals;

    if (m_inverseNormal)
    {
        for (int i = 0; i < nbrV; i++)
        {
            vertices.Add(FVector(sofaVertices[i * 3], sofaVertices[i * 3 + 1], sofaVertices[i * 3 + 2]));
            normals.Add(FVector(-sofaNormals[i * 3], -sofaNormals[i * 3 + 1], -sofaNormals[i * 3 + 2]));
        }
    }
    else
    {
        for (int i = 0; i < nbrV; i++)
        {
            vertices.Add(FVector(sofaVertices[i * 3], sofaVertices[i * 3 + 1], sofaVertices[i * 3 + 2]));
            normals.Add(FVector(sofaNormals[i * 3], sofaNormals[i * 3 + 1], sofaNormals[i * 3 + 2]));
        }
    }
    mesh->UpdateMeshSection(0, vertices, normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>());
}


void ASofaVisualMesh::createMesh()
{
    UE_LOG(SUnreal_log, Warning, TEXT("### createMesh"));

    if (m_sofaMesh == nullptr)
        return;

    // Get topology elements numbers
    int nbrV = m_sofaMesh->getNbVertices();
    int nbrTri = m_sofaMesh->getNbTriangles();
    int nbrQuads = m_sofaMesh->getNbQuads();
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaVisualMesh::createMesh(): nbrV: %d | nbrTri: %d | nbrQuads: %d"), nbrV, nbrTri, nbrQuads);

    if (nbrV <= 0 || nbrV > 100000)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaVisualMesh::createMesh(): Error on the nbrV returned: %d"), nbrV);
        return;
    }

    float* sofaVertices = new float[nbrV*3];
    float* sofaNormals = new float[nbrV*3];
    float* sofaTexCoords = new float[nbrV * 2];
    int* sofaTriangles = new int[nbrTri*3];
    int* sofaQuads = new int[nbrQuads * 3];

    // Get the different buffers
    m_sofaMesh->getVPositions(sofaVertices);
    m_sofaMesh->getVNormals(sofaNormals);
    m_sofaMesh->getTriangles(sofaTriangles);
    m_sofaMesh->getQuads(sofaQuads);
    m_sofaMesh->getVTexCoords(sofaTexCoords);

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
        UV0.Add(FVector2D(sofaTexCoords[i * 2], sofaTexCoords[i * 2 + 1]));

        if (m_inverseNormal)
            normals.Add(FVector(-sofaNormals[i * 3], -sofaNormals[i * 3 + 1], -sofaNormals[i * 3 + 2]));
        else
            normals.Add(FVector(sofaNormals[i * 3], sofaNormals[i * 3 + 1], sofaNormals[i * 3 + 2]));

        tangents.Add(FProcMeshTangent(0, 1, 0));
        vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
    }

    // Add triangles
    for (int i = 0; i < nbrTri; i++)
    {
        Triangles.Add(sofaTriangles[i * 3]);
        Triangles.Add(sofaTriangles[i * 3 + 1]);
        Triangles.Add(sofaTriangles[i * 3 + 2]);
    }

    // Add quads
    for (int i = 0; i < nbrQuads; i++)
    {
        Triangles.Add(sofaQuads[i * 4]);
        Triangles.Add(sofaQuads[i * 4 + 1]);
        Triangles.Add(sofaQuads[i * 4 + 2]);

        Triangles.Add(sofaQuads[i * 4]);
        Triangles.Add(sofaQuads[i * 4 + 2]);
        Triangles.Add(sofaQuads[i * 4 + 3]);
    }
    
    delete[] sofaVertices;
    delete[] sofaNormals;
    delete[] sofaTexCoords;
    delete[] sofaTriangles;
    delete[] sofaQuads;
    
    mesh->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);

    // Enable collision data
    //mesh->ContainsPhysicsTriMeshData(true);
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
