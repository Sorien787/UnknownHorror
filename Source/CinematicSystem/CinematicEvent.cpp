// Fill out your copyright notice in the Description page of Project Settings.


#include "CinematicEvent.h"


// Sets default values
ACinematicEvent::ACinematicEvent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UColliderTriggerType* triggerType = CreateDefaultSubobject<UColliderTriggerType>(TEXT("ColliderTrig"));
	triggerType->CreationMethod = 
	Events.Add(triggerType);
}

// Called when the game starts or when spawned
void ACinematicEvent::BeginPlay()
{
	// for(size_t nEventIndex = 0; nEventIndex < Events.Num(); nEventIndex++)
	// {
	// 	Events[nEventIndex]->
	// }

}

// Called every frame
void ACinematicEvent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

