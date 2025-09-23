// Fill out your copyright notice in the Description page of Project Settings.

#include "DAGNode/SofaDAGNode.h"
#include "SofaUE5.h"
#include "SofaUE5Library/SofaAdvancePhysicsAPI.h"
#include "Base/SofaBaseComponent.h"
#include "SofaVisualMesh.h"
#include "Components/SofaComponent.h"
#include "Engine.h"

// Sets default values
ASofaDAGNode::ASofaDAGNode()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; 
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SofaDAGNode"));
}


void ASofaDAGNode::PostActorCreated()
{
	UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::PostActorCreated: Node: %s"), *this->GetName());
}


void ASofaDAGNode::Destroyed()
{
    if (this->GetFlags() & RF_Transient) {
        return;
    }

    UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::Destroyed: Node: %s"), *this->GetName());
    clearComponents();
    Super::Destroyed();
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


// Called when the game starts or when spawned
void ASofaDAGNode::BeginPlay()
{
    Super::BeginPlay();
    //UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::BeginPlay: Node: %s"), *this->GetName());
}


// Called every frame
void ASofaDAGNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


bool ASofaDAGNode::loadComponents(const TSharedPtr<SofaAdvancePhysicsAPI>& _sofaAPI)
{
    m_sofaAPI = _sofaAPI;

    if (m_sofaAPI == nullptr)
        return false;

    UE_LOG(SUnreal_log, Log, TEXT("## ASofaDAGNode::loadComponents: %s | UniqueID: %s"), *this->GetName(), *this->m_uniqueNameID);

    std::string nodeUniqID = std::string(TCHAR_TO_UTF8(*m_uniqueNameID));
    
    int nbrCompo = m_sofaAPI->getNbrComponentsInNode(nodeUniqID);
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaDAGNode::loadComponents Nbr: %d ##########"), nbrCompo);
    
    if (nbrCompo <= 0)
    {
        // Nothing to do; importantly, do NOT call getDAGNodeComponentsName with index 0
        return true;
    }
    
    UWorld* const World = GetWorld();
    if (World == nullptr)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadComponents: GetWorld return a null pointer"));
        return false;
    }
    
    FTransform SpawnTransform = FTransform::Identity;
    for (int compoId = 0; compoId < nbrCompo; compoId++)
    {
        std::string compoName = "";
        int resCompoName = m_sofaAPI->getDAGNodeComponentName_out(nodeUniqID, compoId, compoName);

        FString fs_compoName(compoName.c_str()); // Convert std::string -> FString
        std::string displayName = "";
        int resDName = m_sofaAPI->getComponentDisplayName_out(compoName, displayName);
        
        std::string baseType = "";
        int resType = m_sofaAPI->getBaseComponentType_out(compoName, baseType);

        if (resDName != 0 || resType != 0)
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaDAGNode::loadComponents: component name and type returns: %d | %d"), resDName, resType);
            continue;
        }
        
        // Deep copy of the strings
        m_componentsNames.push_back(compoName);

        FString fs_displayName(displayName.c_str()); // Convert std::string -> FString
        FString fs_baseType(baseType.c_str()); // Convert std::string -> FString

        FActorSpawnParameters SpawnParams;
        SpawnParams.Name = MakeUniqueObjectName(World, ASofaBaseComponent::StaticClass(), FName(*fs_displayName));

        if (baseType.compare("SofaVisualModel") == 0)
        {
            ASofaBaseComponent* component = World->SpawnActorDeferred<ASofaVisualMesh>(
                ASofaVisualMesh::StaticClass(),
                SpawnTransform,
                this,
                nullptr,
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn
            );

            if (component != nullptr)
            {
                //if (m_log)
                UE_LOG(SUnreal_log, Log, TEXT("### ASofaVisualMesh Created: %s | %s | %s"), *fs_compoName, *fs_displayName, *fs_baseType);

                component->setUniqueNameID(fs_compoName);
                component->setComponentType(fs_baseType);
                component->SetActorLabel(fs_displayName);
                component->setSofaAPI(_sofaAPI);
                component->computeComponent();

                UGameplayStatics::FinishSpawningActor(component, SpawnTransform);
                bool resAttach = component->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
            }
            else
            {
                UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadComponents: component creation is null"));
            }
            //visuMesh->setSofaMesh(mesh);
        }
        else
        {
            UE_LOG(SUnreal_log, Log, TEXT("### USofaComponent Created: %s | %s | %s"), *fs_compoName, *fs_displayName, *fs_baseType);

            //USofaComponent* NewComp = NewObject<USofaComponent>(this);
            //NewComp->RegisterComponent();
        }

        

        // Sleep for 10 ms (0.01 seconds)
        //FPlatformProcess::Sleep(0.01f);
    }

    return true;
}


void ASofaDAGNode::reconnectComponents(const TSharedPtr<SofaAdvancePhysicsAPI>& _sofaAPI)
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
                
                ASofaVisualMesh* visualMesh = dynamic_cast<ASofaVisualMesh*>(actor);
                UE_LOG(SUnreal_log, Warning, TEXT("### ASofaVisualMesh found! | %s"), *visualMesh->getUniqNameID());
                visualMesh->setSofaAPI(_sofaAPI);
            }
        }
    }
}

void ASofaDAGNode::clearComponents()
{
    // clear string names
    m_componentsNames.clear();

    // Destroy any attached child actors
    TArray<AActor*> AttachedActors;
    GetAttachedActors(AttachedActors);

    for (AActor* Child : AttachedActors)
    {
        if (Child)
        {
            Child->Destroy();
        }
    }
}
