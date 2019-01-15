// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActorTest.h"


// Sets default values
AMyActorTest::AMyActorTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActorTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyActorTest::Tick(float DeltaTime)
{
    FVector NewLocation = GetActorLocation();
    float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
    NewLocation.Z += DeltaHeight * 20.0f;       //Scale our height by a factor of 20
    RunningTime += DeltaTime;
    SetActorLocation(NewLocation);

}

