// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EffectTypes.h"
#include "TriggerTypes.h"
#include "CinematicCommon.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CinematicEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerCalled, int, triggerID);

USTRUCT(BlueprintType)
struct DEEPSEAHORROR_API FCinematicEventCollection
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETriggerStage TriggerStage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UITriggerType*> TriggersForNextStage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UITriggerType*> EffectsWhenTriggered;

	// maybe have some effects with an enum:
	// Noreset,
	// ResetIfNoNext
	// ResetAlways
	TArray<size_t> TickableTriggers;
};

UCLASS()
class DEEPSEAHORROR_API ACinematicEvent : public AActor
{
	GENERATED_BODY()
	
	TArray<size_t> TickableEvents;
public:
	// Sets default values for this actor's properties
	ACinematicEvent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UITriggerType*> Events;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
