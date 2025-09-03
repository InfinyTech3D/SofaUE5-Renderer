/*****************************************************************************
 *                 - Copyright (C) - 2022 - InfinyTech3D -                   *
 *                                                                           *
 * This file is part of the SofaUE5-Renderer asset from InfinyTech3D         *
 *                                                                           *
 * GNU General Public License Usage:                                         *
 * This file may be used under the terms of the GNU General                  *
 * Public License version 3. The licenses are as published by the Free       *
 * Software Foundation and appearing in the file LICENSE.GPL3 included in    *
 * the packaging of this file. Please review the following information to    *
 * ensure the GNU General Public License requirements will be met:           *
 * https://www.gnu.org/licenses/gpl-3.0.html.                                *
 *                                                                           *
 * Commercial License Usage:                                                 *
 * Licensees holding valid commercial license from InfinyTech3D may use this *
 * file in accordance with the commercial license agreement provided with    *
 * the Software or, alternatively, in accordance with the terms contained in *
 * a written agreement between you and InfinyTech3D. For further information *
 * on the licensing terms and conditions, contact: contact@infinytech3d.com  *
 *                                                                           *
 * Authors: see Authors.txt                                                  *
 * Further information: https://infinytech3d.com                             *
 ****************************************************************************/
#include "SofaContext.h"
#include "SofaUE5.h"
#include "Engine.h"
#include "CoreMinimal.h"
//#include "SofaVisualMesh.h"
#include "DAGNode/SofaDAGNode.h"
#include "Base/SofaBaseComponent.h"
#include <vector>
#include <string>

//#include "SofaUE5Library/SofaPhysicsAPI.h"
#include "SofaUE5Library/SofaAdvancePhysicsAPI.h"


// Sets default values
ASofaContext::ASofaContext()
    : Dt(0.02)
    , Gravity(0, -9.8, 0)
    , m_isMsgHandlerActivated(true)
    , m_dllLoadStatus(0)
    , m_apiName("")
    , m_isInit(false)
    , m_sofaAPI(nullptr)
    , m_status(-1)
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SofaContext"));
    SetActorScale3D(FVector(10.0, 10.0, 10.0));
    SetActorRotation(FRotator(0.0, 0.0, 270.0));
    m_log = true;
    
    if (m_log && !(this->GetFlags() & RF_Transient))
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::ASofaContext(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
}


void ASofaContext::OnConstruction(const FTransform& Transform)
{
    if (this->GetFlags() & RF_Transient) {
        return;
    }

    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::OnConstruction(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
    Super::OnConstruction(Transform);

#if WITH_EDITOR
    if (m_sofaAPI == nullptr)
    {
        createSofaContext();
    }
#endif
}

void ASofaContext::Destroyed()
{
    if (this->GetFlags() & RF_Transient) {
        return;
    }

    if (m_log)
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::Destroyed(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
    
    // Remove UE5 children actor first before deleting SOFA context
    clearNodeGraph();

    if (m_sofaAPI)
    {
        if (m_log)
            UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::Destroyed(): Delete SofaAdvancePhysicsAPI: %s"), *this->GetName());
        
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::BeginDestroy: m_sofaAPI stop"));
        m_sofaAPI->stop();
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::BeginDestroy: m_sofaAPI stopped"));
        delete m_sofaAPI;
        m_sofaAPI = nullptr;
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::BeginDestroy: m_sofaAPI deleted"));
    }

    Super::Destroyed();
}



// Called when the game starts or when spawned
void ASofaContext::BeginPlay()
{
    if (m_log)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %d ##########"), m_status);
    }

    if (m_sofaAPI == nullptr)
    {
        createSofaContext();
    }

    if (m_sofaAPI)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::BeginPlay: m_sofaAPI start"));
        m_sofaAPI->start();
    }
    else
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::BeginPlay: m_sofaAPI is null"));
    }
    Super::BeginPlay();
}


void ASofaContext::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (m_log)
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::EndPlay(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));

    if (m_sofaAPI)
    {
        m_sofaAPI->stop();
        //m_sofaAPI->activateMessageHandler(false);
    }
    Super::EndPlay(EndPlayReason);
}







void ASofaContext::setDT(float value)
{
    if (m_sofaAPI)
        m_sofaAPI->setTimeStep(value);
}

void ASofaContext::setGravity(FVector value)
{
    if (m_sofaAPI) 
    {
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::setGravity: %f, %f, %f"), value.X, value.Y, value.Z);
        double* grav = new double[3];
        grav[0] = value.X;
        grav[1] = value.Y;
        grav[2] = value.Z;
        m_sofaAPI->setGravity(grav);
    }
}




#if WITH_EDITOR
void ASofaContext::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
    if (PropertyChangedEvent.MemberProperty != nullptr)
    {
        FString MemberName = PropertyChangedEvent.MemberProperty->GetName();
        
        if (MemberName.Compare(TEXT("Gravity")) == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Gravity is %s"), *Gravity.ToString());
            setGravity(Gravity);
        }
        else if (MemberName.Compare(TEXT("Dt")) == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Dt is %f"), Dt);
            setDT(Dt);
        }
        else if (MemberName.Compare(TEXT("filePath")) == 0)
        {
            loadSofaScene();
        }
    }
}
#endif


// Called every frame
void ASofaContext::Tick( float DeltaTime )
{   
    if (m_status != -1 && m_sofaAPI)
    {
        //UE_LOG(LogTemp, Warning, TEXT("## ASofaContext: Tick %d"), m_status);
        m_sofaAPI->step();

        double stime = m_sofaAPI->getTime();
        
        //if (m_isMsgHandlerActivated == true)
        //    catchSofaMessages();
        float value = this->GetGameTimeSinceCreation();
        //UE_LOG(LogTemp, Warning, TEXT("## ASofaContext: Tick: %f %f"), value, stime);
    }
    
    Super::Tick(DeltaTime);
}




void ASofaContext::createSofaContext()
{
    if (m_log)
        UE_LOG(SUnreal_log, Log, TEXT("########## ASofaContext::createSofaContext: %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));   

    if (m_sofaAPI != nullptr) {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext is called with a SofaAPI already created."));
        return;
    }

    FString curPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

    // create a new sofa context through sofaAdvancePhysicsAPI    
    if (m_sofaAPI == nullptr) 
    {
        //TSharedRef<SofaAdvancePhysicsAPI> apiRef(new SofaAdvancePhysicsAPI());
        //m_data.m_sofaAPI = apiRef;
        m_sofaAPI = new SofaAdvancePhysicsAPI();
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaDAGNode::loadComponents TEST 02"));
        // TODO restore that
        //m_sofaAPI->activateMessageHandler(m_isMsgHandlerActivated);
        
        if (m_sofaAPI == nullptr)
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext SofaAdvancePhysicsAPI creation failed."));
            return;
        }

        m_apiName = m_sofaAPI->APIName();

        if (m_log)
        {
            UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::createSofaContext: API Name: %s"), *m_apiName);
            UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::createSofaContext: Status: %d"), m_status);
        }

        // create scene
        UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::createSofaContext: Creating Scene..."));
        int resCreate = m_sofaAPI->createScene();
        

        if (resCreate < 0) {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext: m_sofaAPI createScene result: %d"), resCreate);
            return;
        }
        else
            UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::createSofaContext: m_sofaAPI createScene result: %d"), resCreate);

        
        //load ini file
        //FString iniPath = curPath + "Plugins/SofaUE5/Source/ThirdParty/SofaUE5Library/sofa.ini";
        //const char* pathchar = TCHAR_TO_ANSI(*iniPath);
        //UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: m_sofaAPI load ini file: %s"), *iniPath);
        //m_sofaAPI->loadSofaIni(pathchar);
    }

    if (m_sofaAPI == nullptr)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext: No SofaAdvancePhysicsAPI Available."));
        return;
    }
    
    // Load default plugins
    FString pluginPaths = curPath + "Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64";
    const char* pluginPchar = TCHAR_TO_ANSI(*pluginPaths);
    int resPlug = m_sofaAPI->loadDefaultPlugins(pluginPchar);
    if (resPlug == 0) {
        UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::createSofaContext: loadDefaultPlugin success, returns: %d"), resPlug);
    }
    else {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext: loadDefaultPlugin failed, returns: %d"), resPlug);
    }


    if (!filePath.FilePath.IsEmpty())
        loadSofaScene();
}


void ASofaContext::loadSofaScene()
{
    FString curPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

    if (filePath.FilePath.IsEmpty()) {
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::loadSofaScene: No filePath set."));
        return;
    }

    FString my_filePath = FPaths::ConvertRelativePathToFull(filePath.FilePath);
    const char* pathfile = TCHAR_TO_ANSI(*my_filePath);
    int resScene = m_sofaAPI->load(pathfile);

    if (resScene < 0) {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadSofaScene: Scene loading failed: %s | Error returned: %d"), *my_filePath, resScene);
        return;
    }
    else {
        UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::loadSofaScene: Scene loading with success: %s"), *my_filePath);
    }

    FPlatformProcess::Sleep(0.01f);

    // Pass default scene parameter
   // this->setDT(Dt);
   // this->setGravity(Gravity);

    // Start parsing scene loaded in SOFA
    // Create the actor of the scene:
    if (m_status == -1) {
        this->loadNodeGraph();
    }
    else
    {
        this->reconnectNodeGraph();

    }
    //if (m_isMsgHandlerActivated == true)
    //    catchSofaMessages();

    m_status++;
}


void ASofaContext::loadDefaultPlugin()
{
    if (m_sofaAPI == nullptr)
        return;

    //m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.Component.dll");
    //m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.GL.Component.dll");
    //m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.GUI.Component.dll");

    //if (m_isMsgHandlerActivated == true)
    //    catchSofaMessages();
}


void ASofaContext::loadNodeGraph()
{
    if (m_sofaAPI == nullptr)
        return;
    
    int nbrNode = m_sofaAPI->getNbrDAGNode();
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::loadNodeGraph: Load Node nbr: %d"), nbrNode);

    UWorld* const World = GetWorld();
    if (World == nullptr)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadNodeGraph: GetWorld return a null pointer"));
        return;
    }

    m_dagNodes.clear();
    // First create all Nodes
    for (int nodeId = 0; nodeId < nbrNode; nodeId++)
    {
        std::string nodeUniqID = m_sofaAPI->getDAGNodeAPIName(nodeId);
        std::string nodeDisplayName = m_sofaAPI->getDAGNodeDisplayName(nodeId);

        FString fs_nodeUniqID(nodeUniqID.c_str());
        FString fs_nodeDisplayName(nodeDisplayName.c_str());

        ASofaDAGNode* dagNode = nullptr;
        if (m_status == -1) // create actors
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Name = MakeUniqueObjectName(World, ASofaDAGNode::StaticClass(), FName(*fs_nodeDisplayName));
            SpawnParams.Owner = this;

            dagNode = World->SpawnActor<ASofaDAGNode>(ASofaDAGNode::StaticClass(), SpawnParams);
            if (dagNode != nullptr)
            {                
                //FAttachmentTransformRules att = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
                dagNode->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

                std::string parentName = m_sofaAPI->getDAGNodeParentAPIName(nodeUniqID);
                FString fs_parentName(parentName.c_str());
                dagNode->setUniqueNameID(fs_nodeUniqID);
                dagNode->setParentName(fs_parentName);
                dagNode->SetActorLabel(fs_nodeDisplayName);
                
                if (m_log)
                    UE_LOG(SUnreal_log, Warning, TEXT("### ASofaDAGNode Created: %s | parent: %s | displayName: %s"), *fs_nodeUniqID, *fs_parentName, *fs_nodeDisplayName);
            }
            else
            {
                UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadNodeGraph: ASofaDAGNode actor not created: %s"), *fs_nodeDisplayName);
            }
            m_dagNodes.push_back(dagNode);
        }
    }


    // Reorder Node using Parent
    for (unsigned int i = 0; i < m_dagNodes.size(); ++i)
    {
        ASofaDAGNode* dagNode = m_dagNodes[i];
        const FString& parentName = dagNode->getParentName();        
        //UE_LOG(SUnreal_log, Log, TEXT("## Process: %s | %s"), *dagNode->getUniqNameID(), *parentName);

        auto res = parentName.Compare("None");
        if (res == 0)
            continue;

        for (unsigned int j = 0; j < m_dagNodes.size(); ++j)
        {
            ASofaDAGNode* otherDagNode = m_dagNodes[j];
            if (otherDagNode->getUniqNameID().Compare(parentName) == 0)
            {
                dagNode->AttachToActor(otherDagNode, FAttachmentTransformRules::KeepRelativeTransform);                
            }
        }
    }

    // Load Components Graph
    for (unsigned int i = 0; i < m_dagNodes.size(); ++i)
    {
        m_dagNodes[i]->loadComponents(this->m_sofaAPI);
        //loadComponentsInNode(m_dagNodes[i]);
    }

}


void ASofaContext::reconnectNodeGraph()
{
    UWorld* const World = GetWorld();
    if (World == nullptr)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::reconnectNodeGraph: GetWorld return a null pointer"));
        return;
    }

    TArray<AActor*> ChildActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASofaDAGNode::StaticClass(), ChildActors);

    for (auto actor : ChildActors)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("### ASofaDAGNode found!!"));
        ASofaDAGNode* dagNode = dynamic_cast<ASofaDAGNode*>(actor);

        if (dagNode == nullptr)
        {
            UE_LOG(SUnreal_log, Warning, TEXT("### ASofaContext::reconnectNodeGraph Child actor found which can't be casted into ASofaDAGNode"), actor->GetFName());
        }
        else
        {
            m_dagNodes.push_back(dagNode);
        }
    }

    // reconnect NodeGraph
    for (unsigned int i = 0; i < m_dagNodes.size(); ++i)
    {
        m_dagNodes[i]->reconnectComponents(this->m_sofaAPI);
    }
}


void ASofaContext::clearNodeGraph()
{
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::clearNodeGraph: Number of Node nbr: %d"), m_dagNodes.size());

    for (ASofaDAGNode* node : m_dagNodes)
    {
        if (node != nullptr)
        {
            node->Destroy();
        }
    }
    m_dagNodes.clear();
}



void ASofaContext::catchSofaMessages()
{
    int nbrMsgs = m_sofaAPI->getNbMessages();
    int* type = new int[1];
    type[0] = -1;
    for (int i = 0; i < nbrMsgs; ++i)
    {
        const char* rawMsg = m_sofaAPI->getMessage(i, *type).c_str();
        FString FMessage(rawMsg);

        if (type[0] == -1) {
            continue;
        }
        else if (type[0] == 3) {
            UE_LOG(SofaLog, Warning, TEXT("%s"), *FMessage);
        }
        else if (type[0] == 4) {
            UE_LOG(SofaLog, Error, TEXT("%s"), *FMessage);
        }
        else if (type[0] == 5) {
            UE_LOG(SofaLog, Fatal, TEXT("%s"), *FMessage);
        }
        else {
            UE_LOG(SofaLog, Log, TEXT("%s"), *FMessage);
        }
    }

    m_sofaAPI->clearMessages();

    delete[] type;
}
