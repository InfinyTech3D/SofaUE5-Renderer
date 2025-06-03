// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SofaDAGNode.generated.h"

UCLASS()
class SOFAUE5_API ASofaDAGNode : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASofaDAGNode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
