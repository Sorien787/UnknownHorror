// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CableComponent.h"
#include "GameFramework/Actor.h"
#include "Haze/HazeEffectComponent.h"
#include "HazeCableActor.generated.h"

UCLASS()
class DEEPSEAHORROR_API AHazeCableActor : public AActor, public HazeComponentListener
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHazeCableActor();
	
	virtual void OnHazeStart() override;

	virtual void OnHazeFinish() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	float m_ForceMultiplierX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Setup)
	float m_ForceMultiplierY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCableComponent* m_pCable;

	UPROPERTY(EditAnywhere)
	UHazeEffectComponent* m_pHazeEffector;
};
