// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SofaAdvancePhysicsAPI/SofaAdvancePhysicsAPI.h"
#include "GameFramework/Actor.h"
#include "SofaContext.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(YourLog, Log, All);

UCLASS()
class SOFAUNREAL_API ASofaContext : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASofaContext();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void BeginDestroy() override;

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



private:
    int32 m_dllLoadStatus;

    FString m_apiName;
    bool m_isInit;

    SofaAdvancePhysicsAPI* m_sofaAPI;
};
