// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LegComponent.h"
#include "LegManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API ULegManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULegManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:

	UPROPERTY(EditAnywhere)
	float LegUnplantThreshold{1.0f};

	UPROPERTY(EditAnywhere)
	float RayCastLength{1.0f};
	
	UPROPERTY(EditAnywhere)
	float StepDistance{1.0f};
	
	UPROPERTY(EditAnywhere)
	float StepTime{1.0f};

	UPROPERTY(EditAnywhere)
	float StepHeight{1.0f};
	
	TArray<ULegComponent*> m_controllableLegComponents;
		
};
