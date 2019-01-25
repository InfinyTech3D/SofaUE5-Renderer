// Fill out your copyright notice in the Description page of Project Settings.

#include "SofaContext.h"
#include "SofaUnreal.h"
#include "Engine.h"
#include "CoreMinimal.h"
//#include "SofaVisualMesh.h"
#include <vector>
#include <string>
//#include <SofaAdvancePhysicsAPI/SofaPhysicsBindings.h>
//#include <SofaAdvancePhysicsAPI/SofaPhysicsDefines.h>

DEFINE_LOG_CATEGORY(YourLog);

/*
void *v_sofaDLLHandle;

#pragma region Load DLL
// Method to import a DLL.
bool ASofaContext::importDLL(FString folder, FString name)
{
    FString filePath = *FPaths::GamePluginsDir() + folder + "/" + name;

    if (FPaths::FileExists(filePath))
    {
        v_sofaDLLHandle = FPlatformProcess::GetDllHandle(*filePath); // Retrieve the DLL.
        if (v_sofaDLLHandle != NULL)
        {
            return true;
        }
    }
    return false;	// Return an error.
}
#pragma endregion Load DLL

#pragma region Unload DLL
// If you love something  set it free.
void ASofaContext::freeDLL()
{
    if (v_sofaDLLHandle != NULL)
    {
        //m_FooPluginFunctionDll = NULL;

        FPlatformProcess::FreeDllHandle(v_sofaDLLHandle);
        v_sofaDLLHandle = NULL;
    }
}
#pragma endregion Unload DLL
*/

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

    // create a new sofa context through sofaAdvancePhysicsAPI
    if (m_sofaAPI == NULL)
        m_sofaAPI = new SofaAdvancePhysicsAPI();
}

// Called when the game starts or when spawned
void ASofaContext::BeginPlay()
{
    Super::BeginPlay();
    m_apiName = "test1";
    UE_LOG(YourLog, Warning, TEXT("BeginPlay!!!!!!!!!!!!!!!!, %s"), *m_apiName);

    
    if (m_sofaAPI == NULL)
        return;

    m_apiName = "test4";
    UE_LOG(YourLog, Warning, TEXT("BeginPlay!!!!!!!!!!!!!!!!, %s"), *m_apiName);

    m_apiName = m_sofaAPI->APIName();
    UE_LOG(YourLog, Warning, TEXT("BeginPlay 2 !!!!!!!!!!!!!!!!, %s"), *m_apiName);

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, m_apiName);
   /*

    if (!m_isInit)
    {
        // Create the scene.
        m_sofaAPI->createScene();

        // load scene
        if (!filePath.FilePath.IsEmpty())
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, filePath.FilePath);
            //const char* scenePath = "C:/Users/Belcurves/projects/Unreal Projects/SofaUnreal/Content/SofaScenes/TriangleSurfaceCutting.scn"; //TCHAR_TO_ANSI(*filePath.FilePath);
            const char* scenePath = TCHAR_TO_ANSI(*filePath.FilePath);

            bool resScene = m_sofaAPI->load(scenePath);

            if (resScene)
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "scene ok");
            else
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "scene failed");
        }

        m_isInit = true;
    }
    this->setDT(Dt);
    this->setGravity(Gravity);

    m_sofaAPI->start();

    // Create the actor of the scene:
    int nbr = m_sofaAPI->getNumberObjects();
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::FromInt(nbr));
    */
    /*if (nbr > 0)
        FString type = m_sofaAPI->get3DObjectType(0).c_str();*/

    /*std::vector<std::string> names = m_sofaAPI->get3DObjectList();

    int nbr2 = names.size();
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::FromInt(nbr2));*/


    //m_sofaAPI->get3DObjectType(0).c_str();
    //FString type =
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, type);

    //for (int i = 0; i < nbr; i++)
    //{
    //    const char* name = sofaPhysicsAPI_get3DObjectName(m_sofaAPI, i);
    //    const char* type = sofaPhysicsAPI_get3DObjectType(m_sofaAPI, i);

    //    FString FType = FString(type);
    //    FString FName = FString(name);


    //    //Sofa3DObject * impl = (Sofa3DObject *)sofaPhysicsAPI_get3DObject(m_sofaAPI, name);
    //    //FString type = m_sofaAPI->get3DObjectType(i).c_str();
    //    //FString name = m_sofaAPI->get3DObjectName(i).c_str();

    //    if (FType.Compare("SofaVisual3DObject") == 0)
    //    {
    //        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FType);
    //        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FName);

    //        UWorld* const World = GetWorld();
    //        if (World == NULL)
    //            continue;

    //        ASofaVisualMesh* visuMesh = World->SpawnActor<ASofaVisualMesh>(ASofaVisualMesh::StaticClass());
    //        //visuMesh->SetActorLabel(name);

    //        int res = SAPAPI_SUCCESS;
    //        Sofa3DObject * impl = (Sofa3DObject *)sofaPhysicsAPI_get3DObject(m_sofaAPI, name, &res);
    //        //visuMesh->isStatic = true;
    //        visuMesh->setSofaImpl(impl);
    //    }
    //}

}

void ASofaContext::setDT(float value)
{
    //if (m_sofaAPI)
    //    m_sofaAPI->setTimeStep(value);
}

void ASofaContext::setGravity(FVector value)
{
    //if (m_sofaAPI) {
    //    double* grav = new double[3];
    //    grav[0] = value.X;
    //    grav[1] = value.Y;
    //    grav[2] = value.Z;
    //    m_sofaAPI->setGravity(grav);
    //}
}

void ASofaContext::BeginDestroy()
{
    if (m_sofaAPI)
    {
        m_sofaAPI->stop();
        //delete &m_sofaAPI;
        m_sofaAPI = NULL;
    }

    Super::BeginDestroy();
}

void ASofaContext::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (m_sofaAPI)
    {
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
    FVector NewLocation = GetActorLocation();
    float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
    NewLocation.Z += DeltaHeight * 20.0f;       //Scale our height by a factor of 20
    RunningTime += DeltaTime;
    SetActorLocation(NewLocation);
    Super::Tick(DeltaTime);
    //if (m_sofaAPI)
    //    m_sofaAPI->step();
    //toto = "Truc2";// FString::SanitizeFloat(resMethod);
   
    Super::Tick(DeltaTime);
}
