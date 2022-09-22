// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../HazeEffectComponent.h"
#include "GameFramework/Actor.h"
#include "ClockActor.generated.h"

UCLASS()
class DEEPSEAHORROR_API AClockActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClockActor();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(VisibleAnywhere)
	UHazeEffectComponent* m_pHazeEffector;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnTick(float tickScalar);
	
	float m_fNextTickTime = 1.0f;

	UPROPERTY(EditAnywhere)
	float m_fMinTimeModifier = 1.0f;
	
	UPROPERTY(EditAnywhere)
	float m_fMaxFastTimeModifier = 3.0f;

	UPROPERTY(EditAnywhere)
	float m_fMaxSlowTimeModifier = 3.0f;
};
