// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HazeUtils.h"
#include "GameFramework/Actor.h"
#include "SlammingDoorActor.generated.h"

class UHazeEffectComponent;
UCLASS()
class DEEPSEAHORROR_API ASlammingDoorActor : public AActor
{
	GENERATED_BODY()

	bool m_bIsInSlammableState = true;
	
	bool m_bIsSlamming = false;
public:	
	// Sets default values for this actor's properties
	ASlammingDoorActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetIsInSlammableState(bool set);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStopSlammable();

	UFUNCTION(BlueprintCallable)
	void OnSlamFinished();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnBeginSlam(float slameSize);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHazeEffectComponent* m_pHazeEffectComponent;
	
	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_SlamProbabilityByHazeStrength;
	
	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve m_MaxSlamSizeByHazeModifier;
};
