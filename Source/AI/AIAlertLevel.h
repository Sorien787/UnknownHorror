// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIAlertLevel.generated.h"

class UPerceptionTypeComponent;
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

struct ISharedActorAlertData
{
 ISharedActorAlertData(float _InitPerceptionStrength, float _EventTime, FVector _EventLocation, FAISenseID _SenseID) :
 SenseID(_SenseID),
 EventLocation(_EventLocation),
 EventTime(_EventTime),
 CurrentPerceptionStrength(_InitPerceptionStrength){}

 virtual ~ISharedActorAlertData(){};
 
public:
 
 FAISenseID SenseID;
 FVector EventLocation;
 EAIAlertLevel CurrentAlertLevel {EAIAlertLevel::IDLE};
 float EventTimeoutValue {0.0f};
 float EventTime{0.0f};
 float CurrentPerceptionStrength{0.0f};
 
 virtual EAIAlertLevel GetMaximumAlertLevel() const = 0;
 virtual float GetMaximumPerceptionStrength() const = 0;
};

struct FDefaultActorAlertData : public ISharedActorAlertData
{
 FDefaultActorAlertData() : ISharedActorAlertData(0.0f, 0.0f, FVector::ZeroVector, FAISenseID()){};
 
 virtual EAIAlertLevel GetMaximumAlertLevel() const override {return EAIAlertLevel::IDLE;}
 virtual float GetMaximumPerceptionStrength() const override {return 0.0f;}
};


struct FActorAlertData : public ISharedActorAlertData
{
 FActorAlertData(UPerceptionTypeComponent* _pPerceptionComponent, FAISenseID _SenseID, float _InitPerceptionStrength, float _EventTime, FVector _EventLocation) : ISharedActorAlertData(_InitPerceptionStrength, _EventTime, _EventLocation, _SenseID), pPerceptionComponent(_pPerceptionComponent){}

 UPerceptionTypeComponent* pPerceptionComponent;
 
 virtual EAIAlertLevel GetMaximumAlertLevel() const override;
 virtual float GetMaximumPerceptionStrength() const override;
};


struct FActorAlertActorData : public FActorAlertData
{
 FActorAlertActorData(UPerceptionTypeComponent* _pPerceptionComponent, FAISenseID _SenseID, float _InitPerceptionStrength, float _EventTime, FVector _EventLocation, AActor* actor) : FActorAlertData(_pPerceptionComponent, _SenseID, _InitPerceptionStrength, _EventTime, _EventLocation), Actor(actor){}

 AActor* Actor;
};