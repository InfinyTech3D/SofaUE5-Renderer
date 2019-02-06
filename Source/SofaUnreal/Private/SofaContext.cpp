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

{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

   
}

// Called when the game starts or when spawned
void ASofaContext::BeginPlay()
{
    Super::BeginPlay();
    
    // create a new sofa context through sofaAdvancePhysicsAPI
    if (m_sofaAPI == NULL) {
        m_sofaAPI = new SofaAdvancePhysicsAPI();
        UE_LOG(YourLog, Warning, TEXT("Create SofaAdvancePhysicsAPI"));
    }

    if (m_sofaAPI == NULL)
        return;

    m_apiName = "NoAPI";
    m_apiName = m_sofaAPI->APIName();
    UE_LOG(YourLog, Warning, TEXT("BeginPlay 6 !!!!!!!!!!!!!!!!, %s"), *m_apiName);
     
    

    if (!m_isInit)
    {
        // Create the scene.
        m_sofaAPI->createScene();
        //std::string sharedPath = m_sofaAPI->loadSofaIni("C:/projects/sofa-build/etc/sofa.ini");

        //UE_LOG(YourLog, Warning, TEXT("sharedPath, %s"), *sharedPath.c_str());
        
        const char* filename = "C:/Users/Belcurves/Documents/Unreal Projects/testThird/Plugins/SofaUnreal/Content/SofaScenes/liver-tetra2triangle.scn";
        std::string sfilename = std::string(filename);
        FString fsFilename = FString(sfilename.c_str());
        // load scene
        //if (!filePath.FilePath.IsEmpty())
        {
            UE_LOG(YourLog, Warning, TEXT("filePath.FilePath, %s"), *fsFilename);
            
            //const char* scenePath = "C:/Users/Belcurves/projects/Unreal Projects/SofaUnreal/Content/SofaScenes/TriangleSurfaceCutting.scn"; //TCHAR_TO_ANSI(*filePath.FilePath);
            //const char* scenePath = TCHAR_TO_ANSI(*filePath.FilePath);

            //int resScene = m_sofaAPI->load(scenePath);
            int resScene = m_sofaAPI->load(filename);

            if (resScene == 0) {
                UE_LOG(YourLog, Warning, TEXT("Scene ok"));
            }
            else {
                UE_LOG(YourLog, Warning, TEXT("Scene failed return error: %d"), resScene);
                return;
            }
        }

        m_isInit = true;
    }

    

    this->setDT(Dt);
    this->setGravity(Gravity);

    UE_LOG(YourLog, Warning, TEXT("Start SofaAdvancePhysicsAPI"));
    m_sofaAPI->start();
           
    // Create the actor of the scene:
    int nbr = m_sofaAPI->getNumberObjects();
    UE_LOG(YourLog, Warning, TEXT("Nbr objects: %d"), nbr);
       
    for (int i = 0; i < nbr; i++)
    {
        std::string name = m_sofaAPI->get3DObjectName(i);
        std::string type = m_sofaAPI->get3DObjectType(i);
        
        if (name.empty() || type.empty())
            UE_LOG(YourLog, Warning, TEXT("name empty"));

        FString FType(type.c_str());
        FString FName(name.c_str());
        
        UE_LOG(YourLog, Warning, TEXT("FType, %s"), *FType);
        UE_LOG(YourLog, Warning, TEXT("FName, %s"), *FName);

        if (FType.Compare("SofaVisual3DObject") == 0)
        {
            UE_LOG(YourLog, Warning, TEXT("SofaVisual3DObject"));

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

    UE_LOG(YourLog, Warning, TEXT("ASofaContext::BeginPlay out"));

}

void ASofaContext::setDT(float value)
{
    if (m_sofaAPI)
        m_sofaAPI->setTimeStep(value);
}

void ASofaContext::setGravity(FVector value)
{
    if (m_sofaAPI) {
        double* grav = new double[3];
        grav[0] = value.X;
        grav[1] = value.Y;
        grav[2] = value.Z;
        m_sofaAPI->setGravity(grav);
    }
}

void ASofaContext::BeginDestroy()
{
    if (m_sofaAPI)
    {
        m_sofaAPI->stop();
        //UE_LOG(YourLog, Warning, TEXT("Delete SofaAdvancePhysicsAPI"));
        //delete m_sofaAPI;
        //m_sofaAPI = NULL;
    }

    Super::BeginDestroy();
}

void ASofaContext::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (m_sofaAPI)
    {
        UE_LOG(YourLog, Warning, TEXT("Stop SofaAdvancePhysicsAPI"));
        m_sofaAPI->stop();
    }
    Super::EndPlay(EndPlayReason);
}

void ASofaContext::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
    FString name = PropertyName.GetPlainNameString();
    UE_LOG(YourLog, Warning, TEXT("Dt changed?"), Dt);
    UE_LOG(YourLog, Warning, TEXT("name: "), *name);
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
        m_sofaAPI->step();
    //toto = "Truc2";// FString::SanitizeFloat(resMethod);
   
    Super::Tick(DeltaTime);
}
