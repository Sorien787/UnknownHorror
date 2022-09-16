// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LightModifierComponent.h"
#include "GameFramework/Actor.h"
#include "LightHazeActor.generated.h"

UCLASS()
class DEEPSEAHORROR_API ALightHazeActor : public AActor, public HazeComponentListener
{
	GENERATED_BODY()


public:	
	// Sets default values for this actor's properties
	ALightHazeActor();

	virtual void OnHazeStrengthChanged(float value) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ULightModifierComponent* m_pLightModifier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHazeEffectComponent* m_pHazeEffectComponent;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightPercentOnlineByHazeModifier;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightFlickerFrequencyByHazeModifier;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightMinBrightness;

	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_LightMaxBrightness;
};
