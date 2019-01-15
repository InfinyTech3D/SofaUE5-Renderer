// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActorTest.generated.h"

UCLASS()
class SOFAUNREAL_API AMyActorTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActorTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    float RunningTime;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
