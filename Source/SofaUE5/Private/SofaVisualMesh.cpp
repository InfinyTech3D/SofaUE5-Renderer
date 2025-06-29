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
#include "SofaVisualMesh.h"
#include "SofaUE5.h"
#include "SofaUE5Library/SofaPhysicsAPI.h"
#include "SofaUE5Library/SofaAdvancePhysicsAPI.h"

// Sets default values
ASofaVisualMesh::ASofaVisualMesh()
    : m_isStatic(false)
    , m_inverseNormal(true)
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
    computeComponent();

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
    {
        updateMesh();
    }
}


void ASofaVisualMesh::updateMesh()
{   
    if (m_sofaAPI == nullptr)
        return;

    std::string nodeUniqID = std::string(TCHAR_TO_UTF8(*m_uniqueNameID));

    // Get number of vertices
    int nbrV = m_sofaAPI->getNbVertices(nodeUniqID);

    // Get the different buffers
    float* sofaVertices = new float[nbrV * 3];
    float* sofaNormals = new float[nbrV * 3];
    m_sofaAPI->getVPositions(nodeUniqID, sofaVertices);
    m_sofaAPI->getVNormals(nodeUniqID, sofaNormals);

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


void ASofaVisualMesh::computeComponent()
{
    createMesh();
}

void ASofaVisualMesh::createMesh()
{
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaVisualMesh::createMesh: %s | UniqueID: %s"), *this->GetName(), *this->m_uniqueNameID);

    if (m_sofaAPI == nullptr)
        return;


    std::string nodeUniqID = std::string(TCHAR_TO_UTF8(*m_uniqueNameID));
    // Get topology elements numbers
    int nbrV = m_sofaAPI->getNbVertices(nodeUniqID);
    int nbrTri = m_sofaAPI->getNbTriangles(nodeUniqID);
    int nbrQuads = m_sofaAPI->getNbQuads(nodeUniqID);
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
    m_sofaAPI->getVPositions(nodeUniqID, sofaVertices);
    m_sofaAPI->getVNormals(nodeUniqID, sofaNormals);
    m_sofaAPI->getTriangles(nodeUniqID, sofaTriangles);
    m_sofaAPI->getQuads(nodeUniqID, sofaQuads);
    m_sofaAPI->getVTexCoords(nodeUniqID, sofaTexCoords);

    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaVisualMesh::createMesh(): TOTO1"));


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
