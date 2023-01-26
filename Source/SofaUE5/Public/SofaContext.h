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
#pragma once

#include "GameFramework/Actor.h"
#include "SofaContext.generated.h"

class SofaPhysicsAPI;

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
        bool m_isMsgHandlerActivated = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sofa Parameters")
        bool m_log = false;

protected:
    void catchSofaMessages();

    void createSofaContext();

    void loadDefaultPlugin();


private:
    int32 m_dllLoadStatus;
    FString m_apiName;
    UPROPERTY(SaveGame)
    bool m_isInit;
    
    //UPROPERTY(SaveGame)
    SofaPhysicsAPI* m_sofaAPI = nullptr;
    //TSharedPtr<SofaAdvancePhysicsAPI> m_sofaAPI;
    UPROPERTY(SaveGame)
        int m_status;
};
