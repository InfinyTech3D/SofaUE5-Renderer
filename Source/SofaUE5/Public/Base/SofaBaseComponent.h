// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SofaBaseComponent.generated.h"

class SofaAdvancePhysicsAPI;

UCLASS()
class SOFAUE5_API ASofaBaseComponent : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASofaBaseComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	void setUniqueNameID(const FString& uniqueNameID) { m_uniqueNameID = uniqueNameID; }
	void setComponentType(const FString& type);

	const FString& getComponentType() {
		return m_baseType;
	}

	const FString& getUniqNameID() {
		return m_uniqueNameID;
	}

	void setSofaAPI(SofaAdvancePhysicsAPI* api);

	virtual void computeComponent() {}

protected:
	SofaAdvancePhysicsAPI* m_sofaAPI = nullptr;

	UPROPERTY(SaveGame, VisibleAnywhere)
	FString m_uniqueNameID;
	
	UPROPERTY(SaveGame, VisibleAnywhere)
	FString m_baseType;
};
