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
    if (m_log)
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::ASofaContext(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
}

void ASofaContext::PostActorCreated()
{
    if (m_log)
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
    
    if (this->GetFlags() & RF_Transient) {
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated RF_Transient")); // SUnreal_log: Warning: ######### ASofaContext::PostActorCreated(): SofaContext_0 | Transient ##########
        return;
    }

    {
        if (m_log)
            UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated FINAL() NEW ##########")); // SUnreal_log: Warning: ######### ASofaContext::PostActorCreated(): SofaContext_UAID_2CF05DE6E9C6956E02_1427897879 | Transactional | HasExternalPackage ##########

        createSofaContext();
    }
}

// Called when the game starts or when spawned
void ASofaContext::BeginPlay()
{
    if (m_log)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %d ##########"), m_status);
    }

    createSofaContext();

    if (m_sofaAPI)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::BeginPlay: m_sofaAPI start"));
        m_sofaAPI->start();
    }
    else
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::BeginPlay: m_sofaAPI is null"));
    }
    Super::BeginPlay();
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

void ASofaContext::BeginDestroy()
{
    if (m_log)
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginDestroy(): %s | %s ##########"), *this->GetName(), *LexToString(this->GetFlags()));
    
    if (m_sofaAPI)
    {
        if (m_log)
            UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginDestroy(): Delete SofaAdvancePhysicsAPI: %s"), *this->GetName());
        
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::BeginDestroy: m_sofaAPI stop"));
        m_sofaAPI->stop();
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::BeginDestroy: m_sofaAPI stopped"));
        delete m_sofaAPI;
        m_sofaAPI = NULL;
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::BeginDestroy: m_sofaAPI deleted"));
    }

    Super::BeginDestroy();
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
            //createSofaContext();
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

    FString curPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

    // create a new sofa context through sofaAdvancePhysicsAPI    
    if (m_sofaAPI == nullptr) 
    {
        //TSharedRef<SofaAdvancePhysicsAPI> apiRef(new SofaAdvancePhysicsAPI());
        //m_data.m_sofaAPI = apiRef;
        m_sofaAPI = new SofaAdvancePhysicsAPI();
        
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

    

    FString sFilename = curPath + "Plugins/SofaUE5/Content/SofaScenes/liver.scn";
    filePath.FilePath = sFilename;
    if (filePath.FilePath.IsEmpty()) {
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: No filePath set."));
        return;
    }
   
    FString my_filePath = FPaths::ConvertRelativePathToFull(filePath.FilePath);
    
    //FString fsFilename = FString(pathfile);
    //UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: scene path: %s"), *fsFilename);

    UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::createSofaContext: Loading file: %s"), *my_filePath);
    const char* pathfile = TCHAR_TO_ANSI(*my_filePath);
    int resScene = m_sofaAPI->load(pathfile);

    if (resScene < 0) {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext: Scene loading failed return error: %d"), resScene);
        return;
    }
    else {
        UE_LOG(SUnreal_log, Log, TEXT("## ASofaContext::createSofaContext: Scene loading success."));
    }

   
    // Pass default scene parameter
   // this->setDT(Dt);
   // this->setGravity(Gravity);


    int nbrMesh = m_sofaAPI->getNumberOfMeshIO();    
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: Info: Loaded meshes nbr: %d"), nbrMesh);
    
    // Create the actor of the scene:

    if (m_status == -1) {
        this->loadNodeGraph();
    }
    else
    {
        UWorld* const World = GetWorld();
        if (World == nullptr)
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadNodeGraph: GetWorld return a null pointer"));
            return;
        }

        TArray<AActor*> ChildActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASofaDAGNode::StaticClass(), ChildActors);

        for (auto actor : ChildActors)
        {
            //if (visuMesh->ActorHasTag("SofaVisual"))
            {
                //if (m_log)
                UE_LOG(SUnreal_log, Warning, TEXT("### ACtor found!!"));
                ASofaDAGNode* dagNode = dynamic_cast<ASofaDAGNode*>(actor);
                m_dagNodes.push_back(dagNode);
            }
        }

        // reconnect NodeGraph
        for (unsigned int i = 0; i < m_dagNodes.size(); ++i)
        {
            m_dagNodes[i]->reconnectComponents(this->m_sofaAPI);
        }

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
            SpawnParams.Name = FName(*fs_nodeDisplayName);
            SpawnParams.Owner = this;

            dagNode = World->SpawnActor<ASofaDAGNode>(ASofaDAGNode::StaticClass(), SpawnParams);
            if (dagNode != nullptr)
            {                
                //FAttachmentTransformRules att = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
                bool resAttach = dagNode->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
                if (m_log)
                    UE_LOG(SUnreal_log, Warning, TEXT("### ASofaDAGNode Created: %s | attach: %d"), *fs_nodeDisplayName, resAttach);

                std::string parentName = m_sofaAPI->getDAGNodeParentAPIName(nodeUniqID);
                FString fs_parentName(parentName.c_str());
                dagNode->setUniqueNameID(fs_nodeUniqID);
                dagNode->setParentName(fs_parentName);
                dagNode->SetActorLabel(fs_nodeDisplayName);
                UE_LOG(SUnreal_log, Warning, TEXT("### ASofaDAGNode info: %s | parent: %s"), *fs_nodeUniqID, *fs_parentName);
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
        UE_LOG(SUnreal_log, Log, TEXT("## Process: %s | %s"), *dagNode->getUniqNameID(), *parentName);

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


    //    for (unsigned int meshID = 0; meshID < nbr; meshID++)
    //    {
    //        SofaPhysicsOutputMesh* mesh = m_sofaAPI->getOutputMeshPtr(meshID);
    //        
    //        const char* name = mesh->getName();
    //        FString FName(name);
    //        if (m_log)
    //            UE_LOG(SUnreal_log, Warning, TEXT("## SofaPhysicsOutputMesh: name: %s"), *FName);
    ////        FString FType(type);
    ////        UE_LOG(SUnreal_log, Warning, TEXT("### FName: %s | FType: %s"), *FName, *FType);
    //
    //        UWorld* const World = GetWorld();
    //        if (World == NULL)
    //            continue;
    //        
    //        ASofaVisualMesh* visuMesh = nullptr;
    //        if (m_status == -1) // create actors
    //        {
    //            visuMesh = World->SpawnActor<ASofaVisualMesh>(ASofaVisualMesh::StaticClass());
    //            visuMesh->Tags.Add("SofaVisual");
    //            
    //            FAttachmentTransformRules att = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
    //            visuMesh->AttachToActor(this, att);
    //            if (m_log)
    //                UE_LOG(SUnreal_log, Warning, TEXT("### ACtor Created!!"));
    //        }
    //        else
    //        {
    //            TArray<AActor*> ChildActors;
    //            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASofaVisualMesh::StaticClass(), ChildActors);
    //                                
    //            for (auto actor : ChildActors)
    //            {
    //                if (visuMesh->ActorHasTag("SofaVisual"))
    //                {
    //                    if (m_log)
    //                        UE_LOG(SUnreal_log, Warning, TEXT("### ACtor found!!"));
    //                    
    //                    visuMesh = dynamic_cast<ASofaVisualMesh*>(actor);
    //                    break;
    //                }
    //            }
    //        }
    //            
    //        if (visuMesh != nullptr)
    //        {
    //            if (m_log)
    //                UE_LOG(SUnreal_log, Warning, TEXT("### Set model to Actor!!"));
    //            
    //            visuMesh->setSofaMesh(mesh);
    //        }
    //    }


}


void ASofaContext::loadComponentsInNode(ASofaDAGNode* my_DAGNode)
{
    if (m_sofaAPI == nullptr)
        return;

    UE_LOG(SUnreal_log, Log, TEXT("## loadComponentsInNode: %s"), *my_DAGNode->getUniqNameID());

    std::string nodeUniqID = std::string(TCHAR_TO_UTF8(*my_DAGNode->getUniqNameID()));
    std::string componentList = m_sofaAPI->getDAGNodeComponentsName(nodeUniqID);

    FString fs_componentList(componentList.c_str());
    UE_LOG(SUnreal_log, Log, TEXT("## Process Node: %s | found component List: %s"), *my_DAGNode->getUniqNameID(), *fs_componentList);
    
    std::vector<std::string> components;
    std::istringstream f; f.str(componentList);
    std::string s;
    while (getline(f, s, ',')) {
        components.push_back(s);
    }

    UWorld* const World = GetWorld();
    if (World == nullptr)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadComponentsInNode: GetWorld return a null pointer"));
        return;
    }


    //for (int i=0; i< components.size(); ++i)
    //{
    //    std::string compoName = components[i];
    //    FString fs_compoName(compoName.c_str());
    //    //FString fs_compoName("toto");
    //    UE_LOG(SUnreal_log, Log, TEXT("### ASofaBaseComponent Processing: %d"), i);
    //    UE_LOG(SUnreal_log, Log, TEXT("### ASofaBaseComponent Processing: %s"), *fs_compoName);

    //    ASofaBaseComponent* component = nullptr;
    //    //std::string displayName = m_sofaAPI->getComponentDisplayName(compoName);
    //    ////std::string baseType = m_sofaAPI->getBaseComponentType(compoName);

    //    //if (m_status == -1) // create actors
    //    //{
    //    //    FActorSpawnParameters SpawnParams;
    //    ////    FString fs_compoName(compoName.c_str());
    //    //    //FString fs_displayName(displayName.c_str());
    //    ////  //  FString fs_baseType(baseType.c_str());
    //    ////  
    //    ////  UE_LOG(SUnreal_log, Log, TEXT("### ASofaBaseComponent Created: %s | %s "), *fs_compoName, *fs_displayName);

    //    //    SpawnParams.Name = FName("SofaComponent");

    //    //    component = World->SpawnActor<ASofaBaseComponent>(ASofaBaseComponent::StaticClass(), SpawnParams);
    //    //    if (component != nullptr)
    //    //    {
    //    //        bool resAttach = component->AttachToActor(my_DAGNode, FAttachmentTransformRules::KeepRelativeTransform);
    //    //        //if (m_log)
    //    //        //    UE_LOG(SUnreal_log, Log, TEXT("### ASofaBaseComponent Created: %s | %s | %s"), *fs_compoName, *fs_displayName, *fs_baseType);

    //    //        //component->setUniqueNameID(FString(compoName.c_str()));
    //    //        //component->setComponentType(FString(baseType.c_str()));
    //    //    }
    //    //    else
    //    //    {
    //    //    }
    //    //}
    //}
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
