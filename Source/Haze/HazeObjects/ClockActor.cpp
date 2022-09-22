// Fill out your copyright notice in the Description page of Project Settings.


#include "ClockActor.h"

// Sets default values
AClockActor::AClockActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	m_pHazeEffector = CreateDefaultSubobject<UHazeEffectComponent>(TEXT("Haze Component"));
	check(m_pHazeEffector != nullptr);
}

// Called when the game starts or when spawned
void AClockActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClockActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	m_fNextTickTime -= DeltaTime;

	if (m_fNextTickTime > 0)
		return;

	float newTickTime = 1.0f;
	
	if (!m_pHazeEffector || !m_pHazeEffector->IsHazeActive())
	{
		m_fNextTickTime = newTickTime;
		OnTick(m_fNextTickTime);
		return;
	}
	const float hazeModifierStrength = m_pHazeEffector->GetCurrentHazeModifier();

	

	if (hazeModifierStrength > 0.0f)
	{
		newTickTime *= FMath::Lerp(m_fMinTimeModifier, m_fMaxSlowTimeModifier, FMath::Abs(hazeModifierStrength));		
	}
	else
	{
		newTickTime /= FMath::Lerp(m_fMinTimeModifier, m_fMaxFastTimeModifier, FMath::Abs(hazeModifierStrength));	
	}

	m_fNextTickTime = newTickTime;

	OnTick(m_fNextTickTime);
}

