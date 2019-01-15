// Fill out your copyright notice in the Description page of Project Settings.


#include "mytestClass.h"


// Sets default values for this component's properties
UmytestClass::UmytestClass()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UmytestClass::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UmytestClass::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

//    FVector NewLocation = GetActorLocation();
//    float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
//    NewLocation.Z += DeltaHeight * 20.0f;       //Scale our height by a factor of 20
//    RunningTime += DeltaTime;
//    SetActorLocation(NewLocation);

	// ...
}

