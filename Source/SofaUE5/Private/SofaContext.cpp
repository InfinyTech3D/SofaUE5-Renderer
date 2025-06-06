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
#include "SofaVisualMesh.h"
#include <vector>
#include <string>

#include "SofaUE5Library/SofaPhysicsAPI.h"

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
    
    if (m_log)
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::ASofaContext(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
}

void ASofaContext::PostActorCreated()
{
    if (m_log)
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
    
    if (this->GetFlags() == RF_Transactional) 
    {
        if (m_log)
            UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated FINAL() NEW ##########"));

        createSofaContext();
    }
}

// Called when the game starts or when spawned
void ASofaContext::BeginPlay()
{
    if (m_log)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %d ##########"), m_status);
    }

    createSofaContext();

    if (m_sofaAPI)
    {
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
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginDestroy(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
    
    if (m_sofaAPI)
    {
        if (m_log)
            UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginDestroy(): Delete SofaAdvancePhysicsAPI: %s"), *this->GetName());
        
        m_sofaAPI->stop();
        delete m_sofaAPI;
        m_sofaAPI = NULL;
    }

    Super::BeginDestroy();
}

void ASofaContext::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (m_log)
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::EndPlay(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
    
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
        m_sofaAPI->step();
        
        if (m_isMsgHandlerActivated == true)
            catchSofaMessages();
        //float value = this->GetGameTimeSinceCreation();
        //UE_LOG(LogTemp, Warning, TEXT("## ASofaContext: init: %f %f"), DeltaTime, value);
    }
    

    
    Super::Tick(DeltaTime);
}


void ASofaContext::createSofaContext()
{
    FString curPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

    // create a new sofa context through sofaAdvancePhysicsAPI    
    if (m_sofaAPI == nullptr) 
    {
        //TSharedRef<SofaAdvancePhysicsAPI> apiRef(new SofaAdvancePhysicsAPI());
        //m_data.m_sofaAPI = apiRef;
        m_sofaAPI = new SofaPhysicsAPI(false);
        
        if (m_log)
            UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Create SofaAdvancePhysicsAPI NEW"));
        
        m_sofaAPI->activateMessageHandler(m_isMsgHandlerActivated);
        
        if (m_sofaAPI == nullptr)
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext: SofaAdvancePhysicsAPI creation failed."));
            return;
        }

        m_apiName = m_sofaAPI->APIName();

        if (m_log)
        {
            UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: SofaAdvancePhysicsAPI Name: %s"), *m_apiName);
            UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: init: %d"), m_status);
        }

        // create scene
        m_sofaAPI->createScene();
        
        //load ini file
        FString iniPath = curPath + "Plugins/SofaUE5/Source/ThirdParty/SofaUE5Library/sofa.ini";
        const char* pathchar = TCHAR_TO_ANSI(*iniPath);
        m_sofaAPI->loadSofaIni(pathchar);
    }

    if (m_sofaAPI == nullptr)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext: No SofaAdvancePhysicsAPI Available."));
        return;
    }
    
    // Load default plugins
    loadDefaultPlugin();


    //const char* pathfile = "C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Content/SofaScenes/liver.scn";

    if (filePath.FilePath.IsEmpty()) {
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: No filePath set."));
        return;
    }
   
    FString my_filePath = FPaths::ConvertRelativePathToFull(filePath.FilePath);
    
    //FString fsFilename = FString(pathfile);
    //UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: scene path: %s"), *fsFilename);

    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: filePath.FilePath, %s"), *my_filePath);
    const char* pathfile = TCHAR_TO_ANSI(*my_filePath);
    int resScene = m_sofaAPI->load(pathfile);

    if (resScene > 0) {
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Scene loading success."));
    }
    else {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext: Scene loading failed return error: %d"), resScene);
        return;
    }


    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: init: %d"), m_status);
    // Pass default scene parameter
   // this->setDT(Dt);
   // this->setGravity(Gravity);


    // Create the actor of the scene:
    unsigned int nbr = m_sofaAPI->getNbOutputMeshes();

    if (m_log)
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Nbr objects: %d"), nbr);

    for (unsigned int meshID = 0; meshID < nbr; meshID++)
    {
        SofaPhysicsOutputMesh* mesh = m_sofaAPI->getOutputMeshPtr(meshID);
        
        const char* name = mesh->getName();
        FString FName(name);
        if (m_log)
            UE_LOG(SUnreal_log, Warning, TEXT("## SofaPhysicsOutputMesh: name: %s"), *FName);
//        FString FType(type);
//        UE_LOG(SUnreal_log, Warning, TEXT("### FName: %s | FType: %s"), *FName, *FType);

        UWorld* const World = GetWorld();
        if (World == NULL)
            continue;
        
        ASofaVisualMesh* visuMesh = nullptr;
        if (m_status == -1) // create actors
        {
            visuMesh = World->SpawnActor<ASofaVisualMesh>(ASofaVisualMesh::StaticClass());
            visuMesh->Tags.Add("SofaVisual");
            
            FAttachmentTransformRules att = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
            visuMesh->AttachToActor(this, att);
            if (m_log)
                UE_LOG(SUnreal_log, Warning, TEXT("### ACtor Created!!"));
        }
        else
        {
            TArray<AActor*> ChildActors;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASofaVisualMesh::StaticClass(), ChildActors);
                                
            for (auto actor : ChildActors)
            {
                if (visuMesh->ActorHasTag("SofaVisual"))
                {
                    if (m_log)
                        UE_LOG(SUnreal_log, Warning, TEXT("### ACtor found!!"));
                    
                    visuMesh = dynamic_cast<ASofaVisualMesh*>(actor);
                    break;
                }
            }
        }
            
        if (visuMesh != nullptr)
        {
            if (m_log)
                UE_LOG(SUnreal_log, Warning, TEXT("### Set model to Actor!!"));
            
            visuMesh->setSofaMesh(mesh);
        }
    }

    if (m_isMsgHandlerActivated == true)
        catchSofaMessages();

    //if (m_status == 1)
    m_status++;
}

void ASofaContext::loadDefaultPlugin()
{
    if (m_sofaAPI == nullptr)
        return;

    m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.Component.dll");
    m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.GL.Component.dll");
    m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.GUI.Component.dll");

    if (m_isMsgHandlerActivated == true)
        catchSofaMessages();
}


void ASofaContext::catchSofaMessages()
{
    int nbrMsgs = m_sofaAPI->getNbMessages();
    int* type = new int[1];
    type[0] = -1;
    for (int i = 0; i < nbrMsgs; ++i)
    {
        const char* rawMsg = m_sofaAPI->getMessage(i, *type);
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
