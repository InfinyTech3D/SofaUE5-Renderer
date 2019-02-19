// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <SofaAdvancePhysicsAPI/SofaAdvancePhysicsAPI.h>
#include "GameFramework/Actor.h"
#include "SofaContext.generated.h"


USTRUCT()
struct Fmy_data
{
    GENERATED_BODY()

    
        FString m_test2;
    //TSharedPtr<SofaAdvancePhysicsAPI> m_sofaAPI;
};

UCLASS()
class SOFAUNREAL_API ASofaContext : public AActor
{
    GENERATED_BODY()

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void BeginDestroy() override;

public:
    // Sets default values for this actor's properties
    ASofaContext();


    virtual void PostActorCreated() override;
    
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    // Called every frame
    virtual void Tick( float DeltaSeconds ) override;


    void setDT(float value);

    void setGravity(FVector value);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sofa Parameters")
        FFilePath filePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sofa Parameters")
        float Dt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sofa Parameters")
        FVector Gravity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sofa Parameters")
        bool m_isMsgHandlerActivated;

protected:
    void catchSofaMessages();

    void createSofaContext();


private:
    int32 m_dllLoadStatus;
    float RunningTime;
    FString m_apiName;
    UPROPERTY(SaveGame)
    bool m_isInit;
    
    float m_scaleOsci;
    //UPROPERTY(SaveGame)

    //TSharedPtr<SofaAdvancePhysicsAPI> m_sofaAPI;
    UPROPERTY(SaveGame)
    FString m_test;
    UPROPERTY(SaveGame)
        Fmy_data m_data;
};
