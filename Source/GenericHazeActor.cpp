// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericHazeActor.h"

// Sets default values
AGenericHazeActor::AGenericHazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	m_pHazeEffector = CreateDefaultSubobject<UHazeEffectComponent>(TEXT("Haze Component"));
	check(m_pHazeEffector != nullptr);
}

// Called when the game starts or when spawned
void AGenericHazeActor::BeginPlay()
{
	Super::BeginPlay();

	m_pHazeEffector->m_HazeComponentListeners.AddListener(this, "GenericHazeActor");
}

void AGenericHazeActor::BeginDestroy()
{
	m_pHazeEffector->m_HazeComponentListeners.RemoveListener(this);
}

void AGenericHazeActor::OnHazeStart()
{
	OnHazeStart(m_pHazeEffector->GetCurrentHazeModifier());
}

void AGenericHazeActor::OnHazeFinish()
{
	OnHazeFinish(m_pHazeEffector->GetCurrentHazeModifier());
}

void AGenericHazeActor::OnHazeSetValue(float value)
{
	OnHazeTick(m_pHazeEffector->GetCurrentHazeModifier());
}


