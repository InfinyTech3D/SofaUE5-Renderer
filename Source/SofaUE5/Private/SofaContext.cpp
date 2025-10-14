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
#include "DAGNode/SofaDAGNode.h"
#include "Base/SofaBaseComponent.h"
#include <vector>
#include <string>

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
        UE_LOG(SUnreal_log, Log, TEXT("######### ASofaContext::ASofaContext(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
}


void ASofaContext::OnConstruction(const FTransform& Transform)
{
    if (this->GetFlags() & RF_Transient) {
        return;
    }

    if (m_log)
        UE_LOG(SUnreal_log, Log, TEXT("######### ASofaContext::OnConstruction(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
    
    Super::OnConstruction(Transform);

#if WITH_EDITOR
    UE_LOG(SUnreal_log, Log, TEXT("########## ASofaContext:: BEfore createSofaContext: %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
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
        UE_LOG(SUnreal_log, Log, TEXT("######### ASofaContext::Destroyed(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
    
    // Remove UE5 children actor first before deleting SOFA context
    clearNodeGraph();

    if (m_sofaAPI.IsValid())
    {
        // First stop SOFA simulation 
        m_sofaAPI->stop();

        //if (m_isMsgHandlerActivated == true)
        //    catchSofaMessages();

        if (m_log)
            UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::BeginDestroy: m_sofaAPI stopped"));

        // Deactivate message handler
        m_sofaAPI->activateMessageHandler(false);

        // Free SOFA context Ptr
        m_sofaAPI.Reset();

        if (m_log)
            UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::BeginDestroy: m_sofaAPI deleted"));
    }

    Super::Destroyed();
}



// Called when the game starts or when spawned
void ASofaContext::BeginPlay()
{
    if (m_log)
    {
        UE_LOG(SUnreal_log, Log, TEXT("######### ASofaContext::BeginPlay(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
        UE_LOG(SUnreal_log, Log, TEXT("######### ASofaContext::BeginPlay(): %d ##########"), m_status);
    }

    if (m_sofaAPI == nullptr)
    {
        createSofaContext();
    }

    if (m_sofaAPI)
    {
        if (m_log)
            UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::BeginPlay: m_sofaAPI start"));

		// Start SOFA simulation on UE play
        m_sofaAPI->start();

        if (m_isMsgHandlerActivated == true)
            catchSofaMessages();
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
        UE_LOG(SUnreal_log, Log, TEXT("######### ASofaContext::EndPlay(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));

    if (m_sofaAPI)
    {
        // Stop SOFA simulation on UE play
        m_sofaAPI->stop();

        if (m_isMsgHandlerActivated == true)
            catchSofaMessages();
    }

    Super::EndPlay(EndPlayReason);
}


void ASofaContext::setDT(float value)
{
    //if (m_sofaAPI)
    //    m_sofaAPI->setTimeStep(value);
}

void ASofaContext::setGravity(FVector value)
{
    //if (m_sofaAPI) 
    //{
    //    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::setGravity: %f, %f, %f"), value.X, value.Y, value.Z);
    //    double* grav = new double[3];
    //    grav[0] = value.X;
    //    grav[1] = value.Y;
    //    grav[2] = value.Z;
    //    m_sofaAPI->setGravity(grav);
    //}
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
        // Step SOFA simulation on each UE tick
        m_sofaAPI->step();
        
        //double stime = m_sofaAPI->getTime();
        
        //if (m_isMsgHandlerActivated == true)
        //    catchSofaMessages();
        float value = this->GetGameTimeSinceCreation();
        //UE_LOG(LogTemp, Warning, TEXT("## ASofaContext: Tick: %f %f"), value, stime);
    }
    
    Super::Tick(DeltaTime);
}




void ASofaContext::createSofaContext()
{
    //if (m_log)
    UE_LOG(SUnreal_log, Log, TEXT("########## ASofaContext::createSofaContext: %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));   

    if (m_sofaAPI != nullptr) {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext is called with a SofaAPI already created."));
        return;
    }

    FString curPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

    // create a new sofa context through sofaAdvancePhysicsAPI    
    if (m_sofaAPI == nullptr) 
    {
        m_sofaAPI = MakeShared<SofaAdvancePhysicsAPI>();

        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaDAGNode::loadComponents TEST 28"));
        
        if (m_sofaAPI == nullptr)
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext SofaAdvancePhysicsAPI creation failed."));
            return;
        }

        // activate message handler
        m_sofaAPI->activateMessageHandler(m_isMsgHandlerActivated);

        // Test api Name
        m_apiName = m_sofaAPI->APIName();
        if (m_log)
        {
            UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::createSofaContext: API Name: %s"), *m_apiName);
        }

        // create scene
        int resCreate = m_sofaAPI->createScene();
        
        if (resCreate < 0) {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext: m_sofaAPI createScene result: %d"), resCreate);
            return;
        }
        
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
    
    // Load default plugins at start before loading SOFA scene
    loadDefaultPlugin();


    // If file is already set will load directly the file
    if (!filePath.FilePath.IsEmpty()) {
        loadSofaScene();

        if (m_status != -1)
            this->reconnectNodeGraph();
    }
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

 
    // Pass default scene parameter
   // this->setDT(Dt);
   // this->setGravity(Gravity);
   
    if (m_isMsgHandlerActivated == true)
        catchSofaMessages();
}

void ASofaContext::loadDefaultPlugin()
{
    if (m_sofaAPI == nullptr)
        return;

    bool debugMode = false;

#if (UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT) && !UE_BUILD_SHIPPING
    debugMode = true;
#endif

    FString curPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
    FString pluginPaths;
	if (debugMode)
		pluginPaths = curPath + "Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Debug/";
	else
		pluginPaths = curPath + "Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Release/";

    const char* pluginPchar = TCHAR_TO_ANSI(*pluginPaths);
    int resPlug = m_sofaAPI->loadDefaultPlugins(pluginPchar);
    if (resPlug != 0) {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext: loadDefaultPlugin failed, returns: %d"), resPlug);
    }

    //if (m_isMsgHandlerActivated == true)
    //    catchSofaMessages();
}


// Start parsing scene loaded in SOFA
// Create the actor of the scene:

void ASofaContext::loadNodeGraph()
{
    if (m_sofaAPI == nullptr)
        return;

    clearNodeGraph();

    int nbrNode = m_sofaAPI->getNbrDAGNode();
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::loadNodeGraph: Load Node nbr: %d"), nbrNode);

    UWorld* const World = GetWorld();
    if (World == nullptr)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadNodeGraph: GetWorld return a null pointer"));
        return;
    }

    FTransform SpawnTransform = FTransform::Identity;

    // First create all Nodes
    for (int nodeId = 0; nodeId < nbrNode; nodeId++)
    {
        std::string nodeUniqID = "";
        std::string nodeDisplayName = "";

        int resNameId = m_sofaAPI->getDAGNodeAPIName_out(nodeId, nodeUniqID);
        int resDisplayName = m_sofaAPI->getDAGNodeDisplayName_out(nodeId, nodeDisplayName);

        if (resNameId != 0 || resDisplayName != 0)
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadNodeGraph: node name access return: %d | %d"), resNameId, resDisplayName);
            continue;
        }

        FString fs_nodeUniqID(nodeUniqID.c_str());
        FString fs_nodeDisplayName(nodeDisplayName.c_str());

        FActorSpawnParameters SpawnParams;
        SpawnParams.Name = MakeUniqueObjectName(World, ASofaDAGNode::StaticClass(), FName(*fs_nodeDisplayName));
        SpawnParams.Owner = this;

        ASofaDAGNode* dagNode = World->SpawnActorDeferred<ASofaDAGNode>(
            ASofaDAGNode::StaticClass(),
            SpawnTransform,
            this,
            nullptr,
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn
        );
        
        if (dagNode != nullptr)
        {                
            std::string parentNameId = "";
            int resParentNameId = m_sofaAPI->getDAGNodeParentAPIName_out(nodeUniqID, parentNameId);
            if (resParentNameId != 0)
                UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadNodeGraph: Getting parent name Id returns: %d"), resParentNameId);

            FString fs_parentName(parentNameId.c_str());
            dagNode->setUniqueNameID(fs_nodeUniqID);
            dagNode->setParentName(fs_parentName);
            dagNode->SetActorLabel(fs_nodeDisplayName);
                
            if (m_log)
                UE_LOG(SUnreal_log, Log, TEXT("### ASofaDAGNode Created: %s | parent: %s | displayName: %s"), *fs_nodeUniqID, *fs_parentName, *fs_nodeDisplayName);
            
            UGameplayStatics::FinishSpawningActor(dagNode, SpawnTransform);
            m_dagNodes.Add(dagNode);
        }
        else
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadNodeGraph: ASofaDAGNode actor not created: %s"), *fs_nodeDisplayName);
        }
    }


    // Reorder Node using Parent
    for (auto& WeakDagNode : m_dagNodes)
    {
        ASofaDAGNode* dagNode = WeakDagNode.Get();
        if (dagNode == nullptr)
            continue;

        const FString& parentName = dagNode->getParentName();
        auto res = parentName.Compare("None");
        if (res == 0) {
            dagNode->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
            continue;
        }

        for (auto& WeakOther : m_dagNodes)
        {
            if (ASofaDAGNode* otherDagNode = WeakOther.Get())
            {
                if (otherDagNode->getUniqNameID().Compare(parentName) == 0)
                {
                    dagNode->AttachToActor(otherDagNode, FAttachmentTransformRules::KeepRelativeTransform);
                }
            }
        }
    }

    if (m_isMsgHandlerActivated == true)
        catchSofaMessages();

    m_status++;
}


void ASofaContext::loadSofaComponents()
{
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::loadNodeGraph: Load all components | nbr Nodes: %d"), m_dagNodes.Num());
    // Load Components Graph
    for (auto& WeakDagNode : m_dagNodes)
    {
        if (ASofaDAGNode* dagNode = WeakDagNode.Get())
        {
            dagNode->loadComponents(m_sofaAPI);
        }
    }

    if (m_isMsgHandlerActivated == true)
        catchSofaMessages();
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
            m_dagNodes.Add(dagNode);
        }
    }

    // reconnect NodeGraph
    for (auto& WeakDagNode : m_dagNodes)
    {
        if (ASofaDAGNode* dagNode = WeakDagNode.Get())
        {
            dagNode->reconnectComponents(m_sofaAPI);
        }
    }

    if (m_isMsgHandlerActivated == true)
        catchSofaMessages();
}


void ASofaContext::clearNodeGraph()
{
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::clearNodeGraph: Number of Node nbr: %d"), m_dagNodes.Num());

    for (auto& WeakDagNode : m_dagNodes)
    {
        if (ASofaDAGNode* Node = WeakDagNode.Get())
        {
            // Optionally destroy existing actors
            Node->Destroy();
        }
    }

    // Clear the array
    m_dagNodes.Empty();
}



void ASofaContext::catchSofaMessages()
{
    int nbrMsgs = m_sofaAPI->getNbMessages();
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::catchSofaMessages: nbr message: %d"), nbrMsgs);
    
    for (int i = 0; i < nbrMsgs; ++i)
    {
        std::string rawMsg;
        int type = m_sofaAPI->getMessage_out(i, rawMsg);
        FString FMessage(rawMsg.c_str());

        if (type == -1) {
            continue;
        }
        else if (type == 3) {
            UE_LOG(SofaLog, Warning, TEXT("%s"), *FMessage);
        }
        else if (type == 4) {
            UE_LOG(SofaLog, Error, TEXT("%s"), *FMessage);
        }
        else if (type == 5) {
            UE_LOG(SofaLog, Fatal, TEXT("%s"), *FMessage);
        }
        else {
            UE_LOG(SofaLog, Log, TEXT("%s"), *FMessage);
        }
    }

    m_sofaAPI->clearMessages();
}
