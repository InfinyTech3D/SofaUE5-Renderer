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

#include "Base/SofaBaseComponent.h"
#include "ProceduralMeshComponent.h"
#include "SofaVisualMesh.generated.h"

//DECLARE_LOG_CATEGORY_EXTERN(YourLog, Log, All);

class SofaPhysicsOutputMesh;

UCLASS()
class SOFAUE5_API ASofaVisualMesh : public ASofaBaseComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASofaVisualMesh();

    void setSofaMesh(SofaPhysicsOutputMesh* sofaMesh);

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    //virtual void EndPlay() override;
    void PostActorCreated() override;
    void PostLoad() override;
    
    // Called every frame
    virtual void Tick( float DeltaSeconds ) override;

    void recomputeUV(const TArray<FVector>& vertices, TArray<FVector2D>& UV0);
    void computeBoundingBox(const TArray<FVector>& vertices);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sofa Parameters")
    bool m_isStatic = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sofa Parameters")
    bool m_inverseNormal = true;

    void computeComponent() override;

    void createMesh();
protected:
    void updateMesh();

private:
    UPROPERTY(VisibleAnywhere)
        UProceduralMeshComponent * mesh;

    SofaPhysicsOutputMesh* m_sofaMesh = nullptr;
    FVector m_min;
    FVector m_max;
};
