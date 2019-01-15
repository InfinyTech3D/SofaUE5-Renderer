// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "mytestClass.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOFAUNREAL_API UmytestClass : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UmytestClass();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    float RunningTime;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
