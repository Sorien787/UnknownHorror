// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HazeEffectComponent.h"
#include "GenericHazeActor.generated.h"

UCLASS()
class DEEPSEAHORROR_API AGenericHazeActor : public AActor, public HazeComponentListener
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGenericHazeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	virtual void OnHazeStart() override;

	virtual void OnHazeFinish() override;

	virtual void OnHazeSetValue(float value) override;
private:
	bool m_bHazeEffectActive = false;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnHazeStart(float hazeStrength);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnHazeFinish(float hazeStrength);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHazeTick(float hazeStrength);
	
	UPROPERTY(EditAnywhere)
	UHazeEffectComponent* m_pHazeEffector;

};
