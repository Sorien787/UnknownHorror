// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EffectTypes.h"
#include "TriggerTypes.h"
#include "CinematicCommon.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CinematicEvent.generated.h"


USTRUCT(BlueprintType)
struct DEEPSEAHORROR_API FCinematicEventCollection
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETriggerStage TriggerStage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETriggerStage NextTriggerStage;

	UPROPERTY()
	uint8 ValidTriggers;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UITriggerType*> TriggersForNextStage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UIEffectType*> EffectsWhenTriggered;
	
	TArray<size_t> TickableTriggers;
};

UCLASS()
class DEEPSEAHORROR_API ACinematicEvent : public AActor
{
	GENERATED_BODY()

	ETriggerStage m_CurrentTriggerStage = ETriggerStage::Zero;

	TSet<int> TriggeredEvents; 
	
	TArray<UITriggerType*> TickableEvents;

	TArray<UIEffectItemType*> OngoingEffects;

	TArray<FCinematicEventCollection*> RelevantEvents;

public:
	// Sets default values for this actor's properties
	ACinematicEvent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCinematicEventCollection> Events;

	UPROPERTY(EditAnywhere)
	int m_DefaultItemPriority = 0;
	
	void RegisterOngoingEvent(UIEffectItemType* effect);

	void RelinquishControlForActor(AActor* pActor);

	int GetDefaultItemPriority() const;
protected:

	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;
	
	void LoadNextEventCollections();
	
	UFUNCTION()
	void OnTriggerCalled(int triggerID);

	UFUNCTION()
	void OnTriggerUncalled(int triggerID);
};
