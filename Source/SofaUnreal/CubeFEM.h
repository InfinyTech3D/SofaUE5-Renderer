// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "CubeFEM.generated.h"

UCLASS()
class SOFAUNREAL_API ACubeFEM : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeFEM();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//virtual void EndPlay() override;
	void PostActorCreated() override;
	void PostLoad() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	float RunningTime;
	float resMethod;

	//How long, in seconds, the countdown will run
	UPROPERTY(EditAnywhere)
	float m_scaleOsci;

	void CreateTriangle();

public:
	UFUNCTION(BlueprintCallable, Category = "My DLL Library")
		static bool importDLL(FString folder, FString name);

	UFUNCTION(BlueprintCallable, Category = "My DLL Library")
		static bool importMethodFooPluginFunction();

	UFUNCTION(BlueprintCallable, Category = "My DLL Library")
		static float FooPluginFunctionFromDLL();

	UFUNCTION(BlueprintCallable, Category = "My DLL Library")
		static void freeDLL();

private:
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent * mesh;
};
