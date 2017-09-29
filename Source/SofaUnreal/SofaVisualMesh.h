// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "SofaAdvancePhysicsAPI/SofaAdvancePhysicsAPI.h"
#include "SofaVisualMesh.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(YourLog, Log, All);

UCLASS()
class SOFAUNREAL_API ASofaVisualMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASofaVisualMesh();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//virtual void EndPlay() override;
	void PostActorCreated() override;
	void PostLoad() override;

	void setSofaImpl(Sofa3DObject * impl);
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//How long, in seconds, the countdown will run
	UPROPERTY(EditAnywhere)
		float m_scaleOsci;

	void CreateTriangle();
	float RunningTime;

private:
	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent * mesh;

	Sofa3DObject * m_impl;
};
