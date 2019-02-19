// Fill out your copyright notice in the Description page of Project Settings.

#include "SofaContext.h"
#include "SofaUnreal.h"
#include "Engine.h"
#include "CoreMinimal.h"
#include "SofaVisualMesh.h"
#include <vector>
#include <string>

#include <SofaAdvancePhysicsAPI/SofaPhysicsBindings.h>
//#include <SofaAdvancePhysicsAPI/SofaPhysicsDefines.h>

static int test = 0;
//m_sofaAPI = nullptr;
static SofaAdvancePhysicsAPI* m_sofaAPI = nullptr;
// Sets default values
ASofaContext::ASofaContext()
    : m_dllLoadStatus(0)
    , Dt(0.02)
    , Gravity(0, -9.8, 0)
    , m_apiName("")
    , m_isInit(false)
    //, m_sofaAPI(nullptr)
    //, m_sofaAPI(NULL)
    , m_isMsgHandlerActivated(true)
    , m_test("toto")
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SofaContext"));
    SetActorScale3D(FVector(-10.0, 10.0, 10.0));
    SetActorRotation(FRotator(0.0, 0.0, 270.0));
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::ASofaContext(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::test(): %d ##########"), test);
    test++;
    m_data.m_test2 = "toto2";
}

void ASofaContext::PostActorCreated()
{
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
    
    if (this->GetFlags() == RF_Transactional) {
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated FINAL() ##########"));
        createSofaContext();
        m_test = "tata";
        m_data.m_test2 = "tata2";
    }

    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated(): %s ##########"), *m_test);
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated(): %s ##########"), *m_data.m_test2);
}

// Called when the game starts or when spawned
void ASofaContext::BeginPlay()
{
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));    
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %s ##########"), *m_test);
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %s ##########"), *m_data.m_test2);
    
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
    if (m_sofaAPI) {

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
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginDestroy(): %s ##########"), *m_test);
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginDestroy(): %s ##########"), *m_data.m_test2);
    if (m_sofaAPI)
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginDestroy(): Delete SofaAdvancePhysicsAPI"));
    if (m_sofaAPI)
    {
        //m_sofaAPI->stop();
        //UE_LOG(SUnreal_log, Warning, TEXT("Delete SofaAdvancePhysicsAPI"));
        //delete m_sofaAPI;
        //m_sofaAPI = NULL;
    }

    Super::BeginDestroy();
}

void ASofaContext::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::EndPlay(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::EndPlay(): %s ##########"), *m_test);
    if (m_sofaAPI)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("Stop SofaAdvancePhysicsAPI"));
        m_sofaAPI->stop();
    }
    Super::EndPlay(EndPlayReason);
}

void ASofaContext::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
    FString name = PropertyName.GetPlainNameString();
    UE_LOG(SUnreal_log, Warning, TEXT("Dt changed?"), Dt);
    UE_LOG(SUnreal_log, Warning, TEXT("name: "), *name);
}

// Called every frame
void ASofaContext::Tick( float DeltaTime )
{   
    //FVector NewLocation = GetActorLocation();
    //float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
    //NewLocation.Z += DeltaHeight * 20.0f;       //Scale our height by a factor of 20
    //RunningTime += DeltaTime;
    //SetActorLocation(NewLocation);
    //Super::Tick(DeltaTime);

    if (m_isInit && m_sofaAPI)
    {
        m_sofaAPI->step();
        
        if (m_isMsgHandlerActivated == true)
            catchSofaMessages();
    }
   
    Super::Tick(DeltaTime);
}


void ASofaContext::createSofaContext()
{
    // create a new sofa context through sofaAdvancePhysicsAPI    
    if (m_sofaAPI == nullptr) {
        //TSharedRef<SofaAdvancePhysicsAPI> apiRef(new SofaAdvancePhysicsAPI());
        //m_data.m_sofaAPI = apiRef;
        m_sofaAPI = new SofaAdvancePhysicsAPI();
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Create SofaAdvancePhysicsAPI"));
        m_sofaAPI->activateMessageHandler(m_isMsgHandlerActivated);
    }

    if (m_sofaAPI == nullptr)
        return;

    m_apiName = "NoAPI";
    m_apiName = m_sofaAPI->APIName();
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: SofaAdvancePhysicsAPI Name: %s"), *m_apiName);

    //return;
    if (!m_isInit)
    {
        // Create the scene.
        m_sofaAPI->createScene();
        //std::string sharedPath = m_sofaAPI->loadSofaIni("C:/projects/sofa-build/etc/sofa.ini");

        //UE_LOG(SUnreal_log, Warning, TEXT("sharedPath, %s"), *sharedPath.c_str());

        const char* filename = "C:/Users/Belcurves/Documents/Unreal Projects/testThird/Plugins/SofaUnreal/Content/SofaScenes/liver-tetra2triangle.scn";
        std::string sfilename = std::string(filename);
        FString fsFilename = FString(sfilename.c_str());
        // load scene
        //if (!filePath.FilePath.IsEmpty())
        {
            UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: filePath.FilePath, %s"), *fsFilename);

            //const char* scenePath = "C:/Users/Belcurves/projects/Unreal Projects/SofaUnreal/Content/SofaScenes/TriangleSurfaceCutting.scn"; //TCHAR_TO_ANSI(*filePath.FilePath);
            //const char* scenePath = TCHAR_TO_ANSI(*filePath.FilePath);

            //int resScene = m_sofaAPI->load(scenePath);
            int resScene = m_sofaAPI->load(filename);

            if (resScene == 0) {
                UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Scene loading success."));
            }
            else {
                UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext: Scene loading failed return error: %d"), resScene);
                return;
            }
        }

        m_isInit = true;
    }

    // Pass default scene parameter
    this->setDT(Dt);
    this->setGravity(Gravity);


    // Create the actor of the scene:
    int nbr = m_sofaAPI->getNumberObjects();
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Nbr objects: %d"), nbr);

    for (int i = 0; i < nbr; i++)
    {
        std::string name = m_sofaAPI->get3DObjectName(i);
        std::string type = m_sofaAPI->get3DObjectType(i);

        if (name.empty() || type.empty())
            UE_LOG(SUnreal_log, Error, TEXT("### name empty"));

        FString FType(type.c_str());
        FString FName(name.c_str());

        UE_LOG(SUnreal_log, Warning, TEXT("### FName: %s | FType: %s"), *FName, *FType);

        if (FType.Compare("SofaVisual3DObject") == 0)
        {
            UWorld* const World = GetWorld();
            if (World == NULL)
                continue;

            ASofaVisualMesh* visuMesh = World->SpawnActor<ASofaVisualMesh>(ASofaVisualMesh::StaticClass());
            visuMesh->SetActorLabel(FName);

            FAttachmentTransformRules att = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
            visuMesh->AttachToActor(this, att);
            int res = SAPAPI_SUCCESS;
            Sofa3DObject * impl = (Sofa3DObject *)sofaPhysicsAPI_get3DObject(m_sofaAPI, name.c_str(), &res);
            if (res == SAPAPI_SUCCESS)
                visuMesh->setSofaImpl(impl);
        }
        else
        {
            //    //visuMesh->isStatic = true;
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
        std::string message = m_sofaAPI->getMessage(i, *type);
        FString FMessage(message.c_str());

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
        
        m_sofaAPI->clearMessages();
    }
}
