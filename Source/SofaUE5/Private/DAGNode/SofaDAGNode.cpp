// Fill out your copyright notice in the Description page of Project Settings.

#include "DAGNode/SofaDAGNode.h"
#include "SofaUE5.h"
#include "SofaUE5Library/SofaAdvancePhysicsAPI.h"

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
	UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::PostActorCreated: Node: %s"), *this->GetName());
}


// Called when the game starts or when spawned
void ASofaDAGNode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::BeginPlay: Node: %s"), *this->GetName());
	
}

void ASofaDAGNode::BeginDestroy()
{
    UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::BeginDestroy: Node: %s"), *this->GetName());

    if (m_sofaAPI != nullptr)
    {
		// do not delete, this is part of the SofaContext job. Check how to use shared::Ptr here.
		m_sofaAPI = nullptr;
    }

    Super::BeginDestroy();
}


// Called every frame
void ASofaDAGNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


bool ASofaDAGNode::loadComponents(SofaAdvancePhysicsAPI* _sofaAPI)
{
	m_sofaAPI = _sofaAPI;

	UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::loadComponents: %s | UniqueID: %s"), *this->GetName(), *this->m_uniqueNameID);

	//std::string nodeUniqID = std::string(TCHAR_TO_UTF8(*m_uniqueNameID));
	//std::string componentList = m_sofaAPI->getDAGNodeComponentsName(nodeUniqID);

	//FString fs_componentList(componentList.c_str());
	//UE_LOG(SUnreal_log, Log, TEXT("## Process Node: %s | found component List: %s"), *this->m_uniqueNameID, *fs_componentList);

	return true;
}
