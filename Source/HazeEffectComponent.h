// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HazeUtils.h"
#include "Components/ActorComponent.h"
#include "HazeEffectComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHazeStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHazeFinish);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHazeComponentBreak);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHazeModifierChange, float, HazeModifier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHazeStrengthChange, float, HazeStrength);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UHazeEffectComponent : public UActorComponent, public HazeListener
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHazeEffectComponent();
	ListenerUtils<HazeComponentListener> m_HazeComponentListeners;
private:
	

	float m_CurrentHazeStrength{0.0f};
	float m_CurrentHazeModifier{0.0f};
	float m_LastHazeModifier{0.0f};
	float m_HazeNoisePollLocation{0.0f};
	bool m_HazeReachedThreshold{false};
	float m_RandomSeed{0.0f};
	FVector m_LastPolledLocation{FVector::ZeroVector};

	void UpdateHazeMultiplierValue(float deltaTime);

	float ConvertHazeValueToMultiplier() const;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	virtual void OnRefreshHazeStrength() override;

public:
	UPROPERTY()
		float m_distanceGranularity{0.1f};

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_HazeStrengthToNoiseAmplitude;
	
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_HazeStrengthToNoiseFrequency;

	UPROPERTY(BlueprintAssignable)
	FOnHazeStart m_OnHazeStart;

	UPROPERTY(BlueprintAssignable)
	FOnHazeFinish m_OnHazeFinish;

	UPROPERTY(BlueprintAssignable)
	FOnHazeModifierChange m_OnHazeModifierChanged;

	UPROPERTY(BlueprintAssignable)
	FOnHazeStrengthChange m_OnHazeStrengthChanged;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsHazeActive() const;

	float GetCurrentHazeModifier() const;

	float GetNoiseFrequency() const;
	
	float GetCurrentHazeStrength() const;
};

class IHazeModifierComponent
{
	virtual void OnHazeStrengthChanged(float hazeStrength) = 0;	
};
