// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include <SofaAdvancePhysicsAPI/SofaAdvancePhysicsAPI.h>
#include "GameFramework/Actor.h"
#include "SofaContext.generated.h"



UCLASS()
class SOFAUE5_API ASofaContext : public AActor
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

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

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
    FString m_apiName;
    UPROPERTY(SaveGame)
    bool m_isInit;
    
    //UPROPERTY(SaveGame)
    //SofaAdvancePhysicsAPI* m_sofaAPI;
    //TSharedPtr<SofaAdvancePhysicsAPI> m_sofaAPI;
    UPROPERTY(SaveGame)
        int m_status;
    UPROPERTY(SaveGame)
    FString m_test;
};
