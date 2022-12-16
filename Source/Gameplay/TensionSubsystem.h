// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TensionSubsystem.generated.h"

class ALurkLocationActor;
/**
 * 
 */
UCLASS()
class DEEPSEAHORROR_API UTensionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	virtual TStatId GetStatId() const override;

	virtual void Tick(float DeltaTime) override;
	
public:
	
	UFUNCTION(BlueprintCallable)
	void AddSingleTensionInstance(float addTension);

	UFUNCTION(BlueprintCallable)
	float GetCurrentTension() const;

	UFUNCTION()
	void RegisterLurkLocation(ALurkLocationActor* pLurkActor);

	UFUNCTION(BlueprintCallable)
	ALurkLocationActor* GetLurkLocation(uint8 type, FVector location) const;

	UPROPERTY(EditAnywhere)
	float m_fMaximumTension;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_TensionDecayCurve;

	float m_fCurrentTension{0.0f};

	std::vector<ALurkLocationActor*> m_LurkLocations;
};
