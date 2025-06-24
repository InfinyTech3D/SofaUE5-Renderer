// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include <string>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SofaDAGNode.generated.h"

class SofaAdvancePhysicsAPI;

UCLASS()
class SOFAUE5_API ASofaDAGNode : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASofaDAGNode();

	virtual void PostActorCreated() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void setUniqueNameID(const FString& uniqueNameID) { m_uniqueNameID = uniqueNameID; }
	void setParentName(const FString& parentName) { m_parentName = parentName; }

	bool loadComponents(SofaAdvancePhysicsAPI* _sofaAPI);
	void loadComponents();

	void reconnectComponents(SofaAdvancePhysicsAPI* _sofaAPI);


	const FString& getParentName() {
		return m_parentName;
	}

	const FString& getUniqNameID() {
		return m_uniqueNameID;
	}

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sofa Parameters")
	bool ComponentLoaded = false;

private:
	SofaAdvancePhysicsAPI* m_sofaAPI = nullptr;
	
	UPROPERTY(SaveGame, VisibleAnywhere)
	FString m_uniqueNameID;
	UPROPERTY(SaveGame, VisibleAnywhere)
	FString m_parentName;

	bool m_statusLoaded = false;
	std::vector<std::string> m_componentsNames;
};
