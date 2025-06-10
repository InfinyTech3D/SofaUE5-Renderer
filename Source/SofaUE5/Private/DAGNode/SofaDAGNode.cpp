// Fill out your copyright notice in the Description page of Project Settings.

#include "DAGNode/SofaDAGNode.h"
#include "SofaUE5.h"


// Sets default values
ASofaDAGNode::ASofaDAGNode()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::ASofaDAGNode"));
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SofaDAGNode"));
}

void ASofaDAGNode::PostActorCreated()
{
	UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::PostActorCreated: begin Node: %s"), *this->GetName());
}


// Called when the game starts or when spawned
void ASofaDAGNode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::BeginPlay: begin Node: %s"), *this->GetName());
	
}


// Called every frame
void ASofaDAGNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
