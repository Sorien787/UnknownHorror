// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HazeUtils.h"
#include "Components/ActorComponent.h"
#include "HazeEffectComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHazeStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHazeFinish);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHazeEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UHazeEffectComponent : public UActorComponent, public HazeListener
{
	GENERATED_BODY()

public:	

	UHazeEffectComponent();
	ListenerUtils<HazeComponentListener> m_HazeComponentListeners;
	
private:

	std::unordered_map<int, float> m_HazeIdToSeed;

	float m_CurrentHazeStrength{0.0f};
	float m_HazeNoisePollLocation{0.0f};
	bool m_HazeReachedThreshold{false};
	int m_HazeID{-1};
	FVector m_LastPolledLocation{FVector::ZeroVector};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRefreshHazeGridPosition() override;

public:
	UPROPERTY()
	float m_distanceGranularity{0.1f};

	UPROPERTY(EditAnywhere)
	float m_HazeEffectThreshold{ 0.01f };

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_HazeStrengthToNoiseAmplitude;
	
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_HazeStrengthToNoiseFrequency;
	
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_HazeStrengthToEventProbability;

	UPROPERTY(BlueprintAssignable)
	FOnHazeStart m_OnHazeStart;

	UPROPERTY(BlueprintAssignable)
	FOnHazeFinish m_OnHazeFinish;

	UPROPERTY(BlueprintAssignable)
	FOnHazeEvent m_OnHazeEvent;


	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsHazeActive() const;

	float GetCurrentHazeModifier(int index = 0);

	float GetNoiseFrequency() const;
	
	float GetCurrentHazeStrength() const;
	
	void UpdateHazeMultiplierValue(float deltaTime);

	float ConvertHazeValueToMultiplier() const;

	void RefreshHazeSink();
};
