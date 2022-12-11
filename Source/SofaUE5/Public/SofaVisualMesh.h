// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "SofaVisualMesh.generated.h"

//DECLARE_LOG_CATEGORY_EXTERN(YourLog, Log, All);

class SofaPhysicsOutputMesh;

UCLASS()
class SOFAUE5_API ASofaVisualMesh : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASofaVisualMesh();

    void setSofaMesh(SofaPhysicsOutputMesh* sofaMesh);

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    //virtual void EndPlay() override;
    void PostActorCreated() override;
    void PostLoad() override;

    //void setSofaImpl(Sofa3DObject * impl);

    // Called every frame
    virtual void Tick( float DeltaSeconds ) override;

    void createMesh();
    void recomputeUV(const TArray<FVector>& vertices, TArray<FVector2D>& UV0);
    void computeBoundingBox(const TArray<FVector>& vertices);

    void updateMesh();

    float RunningTime;

    bool isStatic;


private:
    UPROPERTY(VisibleAnywhere)
        UProceduralMeshComponent * mesh;

    SofaPhysicsOutputMesh* m_sofaMesh;
    FVector m_min;
    FVector m_max;
};
