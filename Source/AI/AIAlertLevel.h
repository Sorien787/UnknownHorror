// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "AIAlertLevel.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EAIAlertLevel : uint8
{
 DEFAULT = 0,
 IDLE = 1,
 SUSPICIOUS = 2,
 ALERT = 3,
 INVESTIGATING = 4,
 DETECTION = 5,
 COUNT = 6
};

USTRUCT(BlueprintType)
struct FAlertLevelDetectionThreshold
{
 GENERATED_BODY();
 
 UPROPERTY(EditAnywhere)
 float LowDetectionThreshold;

 UPROPERTY(EditAnywhere)
 float HighDetectionThreshold;
};

USTRUCT(BlueprintType)
struct FActorAlertData
{
 GENERATED_BODY();

 FActorAlertData(EAIAlertLevel inMaxAlertLevel, uint8 inPriority, float maximumInterest) :
  MaxAlertLevel(inMaxAlertLevel),
  CurrentAlertLevel(EAIAlertLevel::IDLE),
  Priority(inPriority),
  MaximumInterest(maximumInterest),
  LastTimeStimulusReceived(0.0f),
  CurrentPerceptionStrength(0.0f){}

 FActorAlertData() :
  MaxAlertLevel(EAIAlertLevel::INVESTIGATING),
  CurrentAlertLevel(EAIAlertLevel::IDLE),
  Priority(0),
  MaximumInterest(0.0f),
  LastTimeStimulusReceived(0.0f),
  CurrentPerceptionStrength(0.0f){}

 UPROPERTY(EditAnywhere)
 EAIAlertLevel MaxAlertLevel;

 UPROPERTY(EditAnywhere)
 EAIAlertLevel CurrentAlertLevel;

 UPROPERTY(EditAnywhere)
 uint8 Priority;

 UPROPERTY(EditAnywhere)
 float MaximumInterest;

 UPROPERTY(EditAnywhere)
 float LastTimeStimulusReceived;

 UPROPERTY(EditAnywhere)
 float CurrentPerceptionStrength;
};

USTRUCT(BlueprintType)
struct FActorAlertActorData
{
 GENERATED_BODY()

 FActorAlertActorData(FActorAlertData data, AActor* actor) : AlertData(data), Actor(actor){}
 FActorAlertActorData() : Actor(nullptr){}

 UPROPERTY(EditAnywhere)
 FActorAlertData AlertData;

 UPROPERTY(EditAnywhere)
 AActor* Actor;
};