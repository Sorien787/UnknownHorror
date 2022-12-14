// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HazeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UHazeComponent : public UActorComponent
{
	GENERATED_BODY()

	int m_HazeID {-1};
	
	void RefreshHazePosition();

	FVector m_LastPolledLocation = FVector::Zero();

	void RefreshHazeSource(float deltaTime);
public:	

	UHazeComponent();

protected:

	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.1", ClampMax = "1000.0", UIMin = "0.1", UIMax = "1000.0"))
	float m_HazeStrengthMultiplier;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.01", ClampMax = "10.0", UIMin = "0.01", UIMax = "10.0"))
	float m_distanceGranularity{0.1f};
};
