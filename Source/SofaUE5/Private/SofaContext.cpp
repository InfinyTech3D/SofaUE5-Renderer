// Fill out your copyright notice in the Description page of Project Settings.

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
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::ASofaContext(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
}

void ASofaContext::PostActorCreated()
{
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
    
    if (this->GetFlags() == RF_Transactional) {
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated FINAL() NEW ##########"));
        createSofaContext();
    }
}

// Called when the game starts or when spawned
void ASofaContext::BeginPlay()
{
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));    
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %d ##########"), m_status);
    
    createSofaContext();

    if (m_sofaAPI)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Start SofaAdvancePhysicsAPI"));
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
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginDestroy(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
    
    if (m_sofaAPI)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginDestroy(): Delete SofaAdvancePhysicsAPI: %s"), *this->GetName());
        m_sofaAPI->stop();
        UE_LOG(SUnreal_log, Warning, TEXT("Delete SofaAdvancePhysicsAPI"));
        delete m_sofaAPI;
        m_sofaAPI = NULL;
    }

    Super::BeginDestroy();
}

void ASofaContext::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::EndPlay(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
    if (m_sofaAPI)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("Stop SofaAdvancePhysicsAPI"));
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
        UE_LOG(LogTemp, Warning, TEXT("PostEditChangeProperty edited: is %s"), *MemberName);
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
        
    //    if (m_isMsgHandlerActivated == true)
    //        catchSofaMessages();
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
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Create SofaAdvancePhysicsAPI NEW"));
        m_sofaAPI->activateMessageHandler(m_isMsgHandlerActivated);
        
        if (m_sofaAPI == nullptr)
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext: SofaAdvancePhysicsAPI creation failed........"));
            return;
        }

        //m_apiName = "NoAPI";
        m_apiName = m_sofaAPI->APIName();
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: SofaAdvancePhysicsAPI Name: %s"), *m_apiName);
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: init: %d"), m_status);

        // create scene
        m_sofaAPI->createScene();
        
        //load ini file
        FString iniPath = curPath + "Plugins/SofaUE5/Source/ThirdParty/SofaUE5Library/sofa.ini";
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: iniPath: %s"), *iniPath);
        const char* pathchar = TCHAR_TO_ANSI(*iniPath);
        //m_sofaAPI->loadSofaIni(pathchar);

        //UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: pathcchar, %s"), pathcchar);
        //const char* resIni = sofaPhysicsAPI_loadSofaIni(m_sofaAPI, pathchar);
        //std::string resIni = m_sofaAPI->loadSofaIni(pathchar);
        //UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: iniPath res: %s"), ANSI_TO_TCHAR(resIni));

        //FString plug1 = curPath + "Plugins/SofaUnreal/Source/ThirdParty/SofaUnrealLibrary/sofa/bin/SofaMiscCollision.dll";
        //FString plug2 = curPath + "Plugins/SofaUnreal/Source/ThirdParty/SofaUnrealLibrary/sofa/bin/SofaSparseSolver.dll";
        //
        //m_sofaAPI->loadPlugin(TCHAR_TO_ANSI(*plug1));
        //m_sofaAPI->loadPlugin(TCHAR_TO_ANSI(*plug2));
    }

    if (m_sofaAPI == nullptr)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext: No SofaAdvancePhysicsAPI Available."));
        return;
    }
    

    const char* pathfile = "C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Content/SofaScenes/liver.scn";

    //if (filePath.FilePath.IsEmpty()) {
    //    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: No filePath set."));
    //    return;
    //}
    m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.Component.dll");
    m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.GL.Component.dll");
    m_sofaAPI->loadPlugin("C:/projects/UnrealEngine/SOFA_test2/Plugins/SofaUE5/Binaries/ThirdParty/SofaUE5Library/Win64/Sofa.GUI.Component.dll");

    //FString my_filePath = FPaths::ConvertRelativePathToFull(filePath.FilePath);

    //const char* filename = "C:/Users/Belcurves/Documents/Unreal Projects/testThird/Plugins/SofaUnreal/Content/SofaScenes/liver-tetra2triangle.scn";
    //std::string sfilename = std::string(filename);
    FString fsFilename = FString(pathfile);
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: scene path: %s"), *fsFilename);

    //UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: filePath.FilePath, %s"), *my_filePath);
   // const char* pathfile = TCHAR_TO_ANSI(*my_filePath);
    int resScene = m_sofaAPI->load(pathfile);

    if (resScene == 0) {
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
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Nbr objects: %d"), nbr);


    //SofaPhysicsOutputMesh* testMesh = meshes[0];
    //unsigned int nbrV = testMesh->getNbVertices();
    //FString FName(name);
    //UE_LOG(SUnreal_log, Warning, TEXT("## SofaPhysicsOutputMesh: name: %d"), nbrV);
    //SofaPhysicsOutputMesh* testMesh = m_sofaAPI->getOutputMeshPtr("test");

    for (unsigned int meshID = 0; meshID < nbr; meshID++)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("### begin loop!!"));
        SofaPhysicsOutputMesh* mesh = m_sofaAPI->getOutputMeshPtr(meshID);

        const std::string& name = mesh->getNameStr();
    //    
    //    //FString FType(type.c_str());
        FString FName("testLiver");
        UE_LOG(SUnreal_log, Warning, TEXT("## SofaPhysicsOutputMesh: name: %s"), *FName);
    //    //UE_LOG(SUnreal_log, Warning, TEXT("### FName: %s | FType: %s"), *FName, *FType);

        UWorld* const World = GetWorld();
        if (World == NULL)
            continue;
            
        ASofaVisualMesh* visuMesh = nullptr;
        if (m_status == -1) // create actors
        {
            visuMesh = World->SpawnActor<ASofaVisualMesh>(ASofaVisualMesh::StaticClass());
            
            visuMesh->SetActorLabel(FName);
                
            FAttachmentTransformRules att = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
            visuMesh->AttachToActor(this, att);
            UE_LOG(SUnreal_log, Warning, TEXT("### ACtor Created!!"));
        }
        else
        {
            //TArray<AActor*> ChildActors;
            //this->GetAllChildActors(ChildActors);

            TArray<AActor*> ChildActors;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASofaVisualMesh::StaticClass(), ChildActors);
                                
            UE_LOG(SUnreal_log, Warning, TEXT("### nbr child: %d"), ChildActors.Num());
            for (auto actor : ChildActors)
            {
                UE_LOG(SUnreal_log, Warning, TEXT("### child name: %s"), *actor->GetName());
                if (FName.Compare(actor->GetActorLabel()) == 0)
                {
                    UE_LOG(SUnreal_log, Warning, TEXT("### ACtor found!!"));
                    visuMesh = dynamic_cast<ASofaVisualMesh*>(actor);
                    break;
                }
            }                
        }
            
        if (visuMesh != nullptr)
        {
            UE_LOG(SUnreal_log, Warning, TEXT("### Set model to Actor!!"));

            visuMesh->setSofaMesh(mesh);
        }

        UE_LOG(SUnreal_log, Warning, TEXT("### end loop!!"));
    }

    if (m_isMsgHandlerActivated == true)
        catchSofaMessages();

    //if (m_status == 1)
    m_status++;
    UE_LOG(SUnreal_log, Warning, TEXT("### out creation!!"));
}

void ASofaContext::catchSofaMessages()
{
    int nbrMsgs = m_sofaAPI->getNbMessages();
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: nbrMsgs: %d"), nbrMsgs);
    
    int* type = new int[1];
    type[0] = -1;
    for (int i = 0; i < nbrMsgs; ++i)
    {
        //std::string message = m_sofaAPI->getMessage(i, *type);
        //const char* mm = message.c_str();
        //FString FMessage(message.c_str());

       /* if (type[0] == -1) {
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
        }*/        
    }

    m_sofaAPI->clearMessages();

    delete[] type;
}
