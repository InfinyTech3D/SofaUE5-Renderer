// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/SofaBaseComponent.h"
#include "SofaUE5Library/SofaAdvancePhysicsAPI.h"

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

void ASofaBaseComponent::setComponentType(const FString& type) 
{ 
	m_baseType = type; 
	//RootComponent = CreateDefaultSubobject<USceneComponent>(*m_baseType);
}

void ASofaBaseComponent::setSofaAPI(SofaAdvancePhysicsAPI* api)
{
	m_sofaAPI = api;
}

