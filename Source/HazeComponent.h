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

	void RefreshHazeSource();
public:	

	UHazeComponent();

protected:

	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_DistanceToStrengthCurve;
	
	UPROPERTY(EditAnywhere)
	float m_HazeStrengthMultiplier;

	UPROPERTY()
	float m_distanceGranularity{0.1f};
};
