// Fill out your copyright notice in the Description page of Project Settings.


#include "CinematicEvent.h"


// Sets default values
ACinematicEvent::ACinematicEvent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACinematicEvent::RegisterOngoingEvent(UIEffectItemType* effect)
{
	// register an ongoing event here
	// relinquish control first, though, to remove old ones.
	RelinquishControlForActor(effect->GetControlledItem());
	OngoingEffects.Add(effect);
}

void ACinematicEvent::RelinquishControlForActor(AActor* pActor)
{
	for (auto& effect : OngoingEffects)
	{
		if (effect->GetControlledItem() != pActor)
			continue;
		// im pretty sure there should only be one?
		effect->CancelItemControl();
		return;
	}
}

int ACinematicEvent::GetDefaultItemPriority() const
{
	return m_DefaultItemPriority;
}

// Called when the game starts or when spawned
void ACinematicEvent::BeginPlay()
{
	Super::BeginPlay();
	for(size_t nIndex = 0; nIndex < Events.Num(); nIndex++)
	{
		FCinematicEventCollection& EventCollection = Events[nIndex];
		for(size_t nIndexEvent = 0; nIndexEvent < EventCollection.TriggersForNextStage.Num(); nIndexEvent++)
		{
			EventCollection.TriggersForNextStage[nIndexEvent]->Initialize(0);
		}
		for(size_t nIndexEvent = 0; nIndexEvent < EventCollection.EffectsWhenTriggered.Num(); nIndexEvent++)
		{
			EventCollection.EffectsWhenTriggered[nIndexEvent]->Initialize(this);
		}
	}
	LoadNextEventCollections();
}

// Called every frame
void ACinematicEvent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (size_t nIndex = 0; nIndex < TickableEvents.Num(); nIndex++)
	{
		TickableEvents[nIndex]->Tick(DeltaTime);
	}
}

void ACinematicEvent::OnTriggerCalled(int triggerID)
{
	for(size_t nIndex = 0; nIndex < RelevantEvents.Num(); nIndex++)
	{
		FCinematicEventCollection& EventCollection = *RelevantEvents[nIndex];
		bool wasTriggered = true;
		for(size_t nIndexEvent = 0; nIndexEvent < EventCollection.TriggersForNextStage.Num(); nIndexEvent++)
		{
			if(EventCollection.TriggersForNextStage[nIndexEvent]->IsConsideredTriggered())
				continue;
			wasTriggered = false;
			break;
		}
		if (!wasTriggered)
			continue;

		m_CurrentTriggerStage = EventCollection.NextTriggerStage;
		for(size_t nIndexEvent2 = 0; nIndexEvent2 < EventCollection.EffectsWhenTriggered.Num(); nIndexEvent2++)
		{
			EventCollection.EffectsWhenTriggered[nIndexEvent2]->OnEffectTriggered();
		}
		LoadNextEventCollections();
		break;
	}
}

void ACinematicEvent::LoadNextEventCollections()
{

	
	for(size_t nIndex = 0; nIndex < RelevantEvents.Num(); nIndex++)
	{
		FCinematicEventCollection& EventCollection = *RelevantEvents[nIndex];
		for(size_t nIndexEvent = 0; nIndexEvent < EventCollection.TriggersForNextStage.Num(); nIndexEvent++)
		{
			EventCollection.TriggersForNextStage[nIndexEvent]->OnTriggerBecomeInvalid(this);
		}
	}
	RelevantEvents.Empty();
	TickableEvents.Empty();
	if (m_CurrentTriggerStage == ETriggerStage::Exit)
		return;
	for(size_t nIndex = 0; nIndex < Events.Num(); nIndex++)
	{
		FCinematicEventCollection& EventCollection = Events[nIndex];
		const bool IsSameTriggerStage = EventCollection.TriggerStage == m_CurrentTriggerStage;
		const bool IsAnyTriggerStage = EventCollection.TriggerStage == ETriggerStage::Any;
		const bool IsZerothTriggerStage = m_CurrentTriggerStage == ETriggerStage::Zero;
		const bool IsValidAnyTriggerStage = IsAnyTriggerStage && !IsZerothTriggerStage;
		
		if (!IsSameTriggerStage && !IsValidAnyTriggerStage)
			continue;
		
		RelevantEvents.Add(&EventCollection);
		for(size_t nIndexEvent = 0; nIndexEvent < EventCollection.TriggersForNextStage.Num(); nIndexEvent++)
		{
			EventCollection.TriggersForNextStage[nIndexEvent]->OnTriggerBecomeValid(this);
			if (EventCollection.TriggersForNextStage[nIndexEvent]->RequiresTick())
				TickableEvents.Add(EventCollection.TriggersForNextStage[nIndexEvent]);
		}
	}
}

void ACinematicEvent::OnTriggerUncalled(int triggerID)
{
 	TriggeredEvents.Remove(triggerID);
}

