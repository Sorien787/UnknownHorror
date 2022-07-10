// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HazeUtils.h"
#include "Components/ActorComponent.h"
#include "HazeEffectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UHazeEffectComponent : public UActorComponent, public HazeListener
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHazeEffectComponent();
private:
	float m_CurrentHazeStrength{0.0f};
	FVector m_LastPolledLocation{FVector::ZeroVector};
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	virtual void OnRefreshHazeStrength() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnHazeStrengthChanged(float hazeStrength);

	UPROPERTY()
		float m_distanceGranularity{0.1f};
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
