// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SofaComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOFAUE5_API USofaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USofaComponent();

	void setUniqueNameID(const FString& uniqueNameID) { m_uniqueNameID = uniqueNameID; }
	void setComponentType(const FString& type);

	const FString& getComponentType() {
		return m_baseType;
	}

	const FString& getUniqNameID() {
		return m_uniqueNameID;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(SaveGame, VisibleAnywhere)
	FString m_uniqueNameID;
	
	UPROPERTY(SaveGame, VisibleAnywhere)
	FString m_baseType;
};
