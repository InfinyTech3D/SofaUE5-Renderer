// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/SofaBaseComponent.h"

// Sets default values
ASofaBaseComponent::ASofaBaseComponent()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SofaBaseComponent"));
}

// Called when the game starts or when spawned
void ASofaBaseComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASofaBaseComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASofaBaseComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

