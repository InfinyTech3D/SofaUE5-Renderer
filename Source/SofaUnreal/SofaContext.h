// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SofaAdvancePhysicsAPI/SofaAdvancePhysicsAPI.h"
#include "GameFramework/Actor.h"
#include "SofaContext.generated.h"


UCLASS()
class SOFAUNREAL_API ASofaContext : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASofaContext();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
		FFilePath filePath;

private:
	int32 m_dllLoadStatus;

	FString m_apiName;

	SofaAdvancePhysicsAPI* m_sofaAPI;
};
