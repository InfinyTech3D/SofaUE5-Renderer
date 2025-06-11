// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SofaVisualModel.h"

// Sets default values
ASofaVisualModel::ASofaVisualModel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASofaVisualModel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASofaVisualModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

