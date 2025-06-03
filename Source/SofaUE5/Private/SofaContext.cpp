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
#include <vector>
#include <string>

#include "SofaUE5Library/SofaPhysicsAPI.h"
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
    SetActorScale3D(FVector(-10.0, 10.0, 10.0));
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
        
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::BeginPlay: m_sofaAPI stop"));
        m_sofaAPI->stop();
        delete m_sofaAPI;
        m_sofaAPI = NULL;
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
        m_sofaAPI->activateMessageHandler(false);
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
            createSofaContext();
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
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: createSofaContext"));
    FString curPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

    // create a new sofa context through sofaAdvancePhysicsAPI    
    if (m_sofaAPI == nullptr) 
    {
        //TSharedRef<SofaAdvancePhysicsAPI> apiRef(new SofaAdvancePhysicsAPI());
        //m_data.m_sofaAPI = apiRef;
        m_sofaAPI = new SofaAdvancePhysicsAPI();
        
        if (m_log)
            UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext Create SofaAdvancePhysicsAPI NEW"));
        
        m_sofaAPI->activateMessageHandler(m_isMsgHandlerActivated);
        
        if (m_sofaAPI == nullptr)
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext SofaAdvancePhysicsAPI creation failed."));
            return;
        }

        m_apiName = m_sofaAPI->APIName();

        if (m_log)
        {
            UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: SofaAdvancePhysicsAPI Name: %s"), *m_apiName);
            UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: init: %d"), m_status);
        }

        // create scene
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: m_sofaAPI creating Scene"));
        int resCreate = m_sofaAPI->createScene();
        

        if (resCreate < 0) {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext: m_sofaAPI createScene result: %d"), resCreate);
            return;
        }
        else
            UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: m_sofaAPI createScene result: %d"), resCreate);

        
        //load ini file
        FString iniPath = curPath + "Plugins/SofaUE5/Source/ThirdParty/SofaUE5Library/sofa.ini";
        const char* pathchar = TCHAR_TO_ANSI(*iniPath);
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: m_sofaAPI load ini file: %s"), *iniPath);
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
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: m_sofaAPI loadDefaultPlugin result: %d"), resPlug);
    }
    else {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext: m_sofaAPI loadDefaultPlugin result: %d"), resPlug);
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

    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: filePath.FilePath, %s"), *my_filePath);
    const char* pathfile = TCHAR_TO_ANSI(*my_filePath);
    int resScene = m_sofaAPI->load(pathfile);

    if (resScene < 0) {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext: Scene loading failed return error: %d"), resScene);
        return;
    }
    else {
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: Scene loading success."));
    }


    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: init: %d"), m_status);
    
    // Pass default scene parameter
   // this->setDT(Dt);
   // this->setGravity(Gravity);


    int nbrMesh = m_sofaAPI->getNumberOfMeshIO();
    
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::createSofaContext: m_sofaAPI load meshes nbr: %d"), nbrMesh);
    UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::createSofaContext: Scene loading failed return error: %d"), resScene);
    
    // Create the actor of the scene:

    this->loadNodeGraph();

    //unsigned int nbr = 0;//m_sofaAPI->getNbOutputMeshes();

    //if (m_log)
    //    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Nbr objects: %d"), nbr);





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

    //if (m_isMsgHandlerActivated == true)
    //    catchSofaMessages();

    //if (m_status == 1)
    m_status++;
}

void ASofaContext::loadDefaultPlugin()
{
    if (m_sofaAPI == nullptr)
        return;

    //m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.Component.dll");
    //m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.GL.Component.dll");
    //m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.GUI.Component.dll");

    if (m_isMsgHandlerActivated == true)
        catchSofaMessages();
}


void ASofaContext::loadNodeGraph()
{
    if (m_sofaAPI == nullptr)
        return;
    
    int nbrNode = m_sofaAPI->getNbrDAGNode();
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext::loadNodeGraph: m_sofaAPI load Node nbr: %d"), nbrNode);

    UWorld* const World = GetWorld();
    if (World == nullptr)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadNodeGraph: GetWorld return a null pointer"));
        return;
    }

    for (int nodeId = 0; nodeId < nbrNode; nodeId++)
    {
        std::string nodeUniqID = m_sofaAPI->getDAGNodeAPIName(nodeId);
        std::string nodeDisplayName = m_sofaAPI->getDAGNodeDisplayName(nodeId);

        FString fs_nodeUniqID(nodeUniqID.c_str());
        FString fs_nodeDisplayName(nodeDisplayName.c_str());



        ASofaDAGNode* dagNode = nullptr;
        if (m_status == -1) // create actors
        {
            dagNode = World->SpawnActor<ASofaDAGNode>(ASofaDAGNode::StaticClass());
            if (dagNode != nullptr)
            {
                
                FAttachmentTransformRules att = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
                dagNode->AttachToActor(this, att);
                if (m_log)
                    UE_LOG(SUnreal_log, Warning, TEXT("### ASofaDAGNode Created: %s"), *fs_nodeDisplayName);

                dagNode->Rename(*fs_nodeDisplayName);
            }
            else
            {
                UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext::loadNodeGraph: ASofaDAGNode actor not created: %s"), *fs_nodeDisplayName);
            }
            
        }
    }
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
