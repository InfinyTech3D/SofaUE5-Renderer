// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SofaBaseComponent.generated.h"

UCLASS()
class SOFAUE5_API ASofaBaseComponent : public ACharacter
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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	void setUniqueNameID(const FString& uniqueNameID) { m_uniqueNameID = uniqueNameID; }
	void setComponentType(const FString& type) { m_baseType = type; }

	const FString& getComponentType() {
		return m_baseType;
	}

	const FString& getUniqNameID() {
		return m_uniqueNameID;
	}

private:
	FString m_uniqueNameID;
	FString m_baseType;
};
