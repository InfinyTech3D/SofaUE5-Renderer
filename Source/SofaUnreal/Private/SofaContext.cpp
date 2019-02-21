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

// Sets default values
ASofaContext::ASofaContext()
    : m_dllLoadStatus(0)
    , Dt(0.02)
    , Gravity(0, -9.8, 0)
    , m_apiName("")
    , m_isInit(false)
    , m_sofaAPI(NULL)
    , m_status(-1)
    , m_isMsgHandlerActivated(true)
    , m_test("NotInit")
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
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated FINAL() ##########"));
        createSofaContext();
        m_test = "Init";
    }

    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::PostActorCreated(): %s ##########"), *m_test);
}

// Called when the game starts or when spawned
void ASofaContext::BeginPlay()
{
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %s | %s ##########"), *this->GetName(), *intToHexa(this->GetFlags()));    
    UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginPlay(): %s ##########"), *m_test);
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

    if (m_sofaAPI)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("######### ASofaContext::BeginDestroy(): Delete SofaAdvancePhysicsAPI: %s"), *this->GetName());
        //m_sofaAPI->stop();
        //UE_LOG(SUnreal_log, Warning, TEXT("Delete SofaAdvancePhysicsAPI"));
        delete m_sofaAPI;
        m_sofaAPI = NULL;
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
        m_sofaAPI->activateMessageHandler(false);
    }
    Super::EndPlay(EndPlayReason);
}


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

// Called every frame
void ASofaContext::Tick( float DeltaTime )
{   
    if (m_status != -1 && m_sofaAPI)
    {
        m_sofaAPI->step();
        
        if (m_isMsgHandlerActivated == true)
            catchSofaMessages();
    }
   
    Super::Tick(DeltaTime);
}


void ASofaContext::createSofaContext()
{
    FString curPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

    // create a new sofa context through sofaAdvancePhysicsAPI    
    if (m_sofaAPI == nullptr) {
        //TSharedRef<SofaAdvancePhysicsAPI> apiRef(new SofaAdvancePhysicsAPI());
        //m_data.m_sofaAPI = apiRef;
        m_sofaAPI = new SofaAdvancePhysicsAPI();
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Create SofaAdvancePhysicsAPI"));
        m_sofaAPI->activateMessageHandler(m_isMsgHandlerActivated);

        if (m_sofaAPI == nullptr)
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext: SofaAdvancePhysicsAPI creation failed."));
            return;
        }

        m_apiName = "NoAPI";
        m_apiName = m_sofaAPI->APIName();
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: SofaAdvancePhysicsAPI Name: %s"), *m_apiName);
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: init: %d"), m_status);

        // create scene
        m_sofaAPI->createScene();
        
        // load ini file
        FString iniPath = curPath + "Plugins/SofaUnreal/Source/ThirdParty/SofaUnrealLibrary/sofa/etc/sofa.ini";
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: iniPath: %s"), *iniPath);
        const char* pathchar = TCHAR_TO_ANSI(*iniPath);

        //UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: pathcchar, %s"), pathcchar);
        const char* resIni = sofaPhysicsAPI_loadSofaIni(m_sofaAPI, pathchar);
        //std::string resIni = m_sofaAPI->loadSofaIni(pathchar);
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: iniPath res: %s"), ANSI_TO_TCHAR(resIni));

        FString plug1 = curPath + "Plugins/SofaUnreal/Source/ThirdParty/SofaUnrealLibrary/sofa/bin/SofaMiscCollision.dll";
        FString plug2 = curPath + "Plugins/SofaUnreal/Source/ThirdParty/SofaUnrealLibrary/sofa/bin/SofaSparseSolver.dll";
        
        m_sofaAPI->loadPlugin(TCHAR_TO_ANSI(*plug1));
        m_sofaAPI->loadPlugin(TCHAR_TO_ANSI(*plug2));
    }

    if (m_sofaAPI == nullptr)
    {
        UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext: No SofaAdvancePhysicsAPI Available."));
        return;
    }
    

    if (filePath.FilePath.IsEmpty()) {
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: No filePath set."));
        return;
    }
   

    FString my_filePath = FPaths::ConvertRelativePathToFull(filePath.FilePath);

    //const char* filename = "C:/Users/Belcurves/Documents/Unreal Projects/testThird/Plugins/SofaUnreal/Content/SofaScenes/liver-tetra2triangle.scn";
    //std::string sfilename = std::string(filename);
    //FString fsFilename = FString(sfilename.c_str());


    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: filePath.FilePath, %s"), *my_filePath);
    const char* pathfile = TCHAR_TO_ANSI(*my_filePath);
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
    this->setDT(Dt);
    this->setGravity(Gravity);


    // Create the actor of the scene:
    int nbr = m_sofaAPI->getNumberObjects();
    UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: Nbr objects: %d"), nbr);

    for (int i = 0; i < nbr; i++)
    {
        UE_LOG(SUnreal_log, Warning, TEXT("### begin loop!!"));
        std::string name = m_sofaAPI->get3DObjectName(i);

        Sofa3DObject * impl = nullptr;
        int res = m_sofaAPI->get3DObject(name, impl);

        //Sofa3DObject * impl = (Sofa3DObject *)sofaPhysicsAPI_get3DObject(m_sofaAPI, name.c_str(), &res);

        if (res != SAPAPI_SUCCESS)
        {
            UE_LOG(SUnreal_log, Error, TEXT("## ASofaContext: acces to object: %d return error: %d"), i, res);
            continue;
        }
               
        UE_LOG(SUnreal_log, Warning, TEXT("## ASofaContext: acces res: %d"), res);
        const std::string& type = impl->getObjectType();
        //
        //std::string type = m_sofaAPI->get3DObjectType(i);
        
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
                    if (FName.Compare(actor->GetName()) == 0)
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

                visuMesh->setSofaImpl(impl);
            }
        }
        else
        {
            //    //visuMesh->isStatic = true;
        }
        UE_LOG(SUnreal_log, Warning, TEXT("### end loop!!"));
    }

   // if (m_isMsgHandlerActivated == true)
   //     catchSofaMessages();

    //if (m_status == 1)
    m_status++;
    UE_LOG(SUnreal_log, Warning, TEXT("### out creation!!"));
}

void ASofaContext::catchSofaMessages()
{
    int nbrMsgs = m_sofaAPI->getNbMessages();
    int* type = new int[1];
    type[0] = -1;
    for (int i = 0; i < nbrMsgs; ++i)
    {
        const char* c_msg = sofaPhysicsAPI_getMessage(m_sofaAPI, i, type);
        //std::string message = m_sofaAPI->getMessage(i, *type);
        FString FMessage(c_msg);

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
