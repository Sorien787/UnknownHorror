// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HazeUtils.h"
#include "Components/ActorComponent.h"
#include "ItemControl/ItemControlComponent.h"
#include "HazeEffectComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHazeStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHazeFinish);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHazeEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UHazeEffectComponent : public UActorComponent, public HazeListener, public IItemControlRequester
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

	virtual void BeginPlay() override;

	virtual void OnRefreshHazeGridPosition() override;

public:
	UPROPERTY()
	float m_distanceGranularity{0.1f};

	UPROPERTY(EditAnywhere)
	float m_HazeEffectThreshold{ 0.01f };

	UPROPERTY(EditAnywhere)
	int m_ItemControlPriority = 0;
	
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

	// IItemControlRequester
	virtual void OnItemControlGranted_Implementation(AActor* pControlledActor) override;
	
	virtual void OnItemControlLost_Implementation(AActor* pControlledActor) override;

	virtual int GetPriority() const override;
	// ~IItemControlRequester
	
	// UActorComponent
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// ~UActorComponent
	bool IsHazeActive() const;

	float GetCurrentHazeModifier(int index = 0);

	float GetNoiseFrequency() const;
	
	float GetCurrentHazeStrength() const;

private:
	
	void UpdateHazeMultiplierValue(float deltaTime);

	float ConvertHazeValueToMultiplier() const;

	void RefreshHazeSink();
};
