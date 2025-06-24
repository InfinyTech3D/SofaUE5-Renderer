// Fill out your copyright notice in the Description page of Project Settings.

#include "DAGNode/SofaDAGNode.h"
#include "SofaUE5.h"
#include "SofaUE5Library/SofaAdvancePhysicsAPI.h"
#include "Base/SofaBaseComponent.h"
#include "SofaVisualMesh.h"
#include "Engine.h"

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


#if WITH_EDITOR
void ASofaDAGNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.MemberProperty != nullptr)
    {
        FString MemberName = PropertyChangedEvent.MemberProperty->GetName();
        UE_LOG(LogTemp, Warning, TEXT("PostEditChangeProperty: %s"), *MemberName);

		if (MemberName.Compare(TEXT("ComponentLoaded")) == 0)
		{
			loadComponents();
		}
    }
}
#endif


// Called every frame
void ASofaDAGNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


bool ASofaDAGNode::loadComponents(SofaAdvancePhysicsAPI* _sofaAPI)
{
	m_sofaAPI = _sofaAPI;

    if (m_sofaAPI == nullptr)
        return false;


	UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::loadComponents: %s | UniqueID: %s"), *this->GetName(), *this->m_uniqueNameID);

	std::string nodeUniqID = std::string(TCHAR_TO_UTF8(*m_uniqueNameID));
	m_componentsNames = m_sofaAPI->getDAGNodeComponentsNames(nodeUniqID);

    UWorld* const World = GetWorld();
    if (World == nullptr)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadComponents: GetWorld return a null pointer"));
        return false;
    }

    for (int i=0; i< m_componentsNames.size(); ++i)
    {
        UE_LOG(SUnreal_log, Log, TEXT("### ASofaBaseComponent Processing: %d START"), i);
        std::string compoName = m_componentsNames[i];
        FString fs_compoName(compoName.c_str());
        
        UE_LOG(SUnreal_log, Log, TEXT("### ASofaBaseComponent Processing: %d | %s"), i, *fs_compoName);

        UE_LOG(SUnreal_log, Log, TEXT("### TOTO1"));
        std::string displayName = m_sofaAPI->getComponentDisplayName(compoName);
        UE_LOG(SUnreal_log, Log, TEXT("### TOTO2"));
        std::string baseType = m_sofaAPI->getBaseComponentType(compoName);
        UE_LOG(SUnreal_log, Log, TEXT("### TOTO3"));
        FString fs_displayName(displayName.c_str());
        FString fs_baseType(baseType.c_str());
        UE_LOG(SUnreal_log, Log, TEXT("### TOTO4"));
        FActorSpawnParameters SpawnParams;
        SpawnParams.Name = FName(*fs_displayName);

        UE_LOG(SUnreal_log, Log, TEXT("### ASofaBaseComponent Created: %s | %s "), *fs_compoName, *fs_displayName);
        ASofaBaseComponent* component = nullptr;
        UE_LOG(SUnreal_log, Log, TEXT("### TOTO5"));
        if (baseType.compare("SofaVisualModel") == 0)
        {
            component = World->SpawnActor<ASofaVisualMesh>(ASofaVisualMesh::StaticClass(), SpawnParams);
            //visuMesh->setSofaMesh(mesh);
         
        }
        else
        {
            component = World->SpawnActor<ASofaBaseComponent>(ASofaBaseComponent::StaticClass(), SpawnParams);
        }
        UE_LOG(SUnreal_log, Log, TEXT("### TOTO6"));
        if (component != nullptr)
        {
            bool resAttach = component->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
            //if (m_log)
            UE_LOG(SUnreal_log, Log, TEXT("### ASofaBaseComponent Created: %s | %s | %s"), *fs_compoName, *fs_displayName, *fs_baseType);

            component->setUniqueNameID(FString(compoName.c_str()));
            component->setComponentType(FString(baseType.c_str()));
            component->SetActorLabel(fs_displayName);
            component->setSofaAPI(_sofaAPI);
            component->computeComponent();
            UE_LOG(SUnreal_log, Log, TEXT("### TOTO7"));
        }
        else
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadComponents: component creation is null"));
        }
        UE_LOG(SUnreal_log, Log, TEXT("### ASofaBaseComponent Processing: %d END"), i);
    }

	return true;
}

void ASofaDAGNode::loadComponents()
{
	if (m_statusLoaded == true) // only once
		return;

	UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::loadComponents: %s | UniqueID: %s"), *this->GetName(), *this->m_uniqueNameID);
	
	std::string nodeUniqID = std::string(TCHAR_TO_UTF8(*m_uniqueNameID));
	m_componentsNames = m_sofaAPI->getDAGNodeComponentsNames(nodeUniqID);
	//std::string componentList = m_sofaAPI->getDAGNodeComponentsName(nodeUniqID);

	//FString fs_componentList(componentList.c_str());
	UE_LOG(SUnreal_log, Log, TEXT("## Process Node: %s | m_componentsNames size: %d"), *this->m_uniqueNameID, m_componentsNames.size());


	m_statusLoaded = true;
}

void ASofaDAGNode::reconnectComponents(SofaAdvancePhysicsAPI* _sofaAPI)
{
    m_sofaAPI = _sofaAPI;

    if (m_sofaAPI == nullptr)
    {
        UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::reconnectComponents: %s | UniqueID: %s | m_sofaAPI is null"), *this->GetName(), *this->m_uniqueNameID);
    }
    else
    {
        UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::reconnectComponents: %s | UniqueID: %s | m_sofaAPI is NOT null"), *this->GetName(), *this->m_uniqueNameID);
        UWorld* const World = GetWorld();
        if (World == nullptr)
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadNodeGraph: GetWorld return a null pointer"));
            return;
        }

        TArray<AActor*> ChildActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASofaVisualMesh::StaticClass(), ChildActors);

        for (auto actor : ChildActors)
        {
            //if (visuMesh->ActorHasTag("SofaVisual"))
            {
                //if (m_log)
                UE_LOG(SUnreal_log, Warning, TEXT("### ACtor found!!"));
                ASofaVisualMesh* visualMesh = dynamic_cast<ASofaVisualMesh*>(actor);
                visualMesh->setSofaAPI(_sofaAPI);
            }
        }
    }
}
