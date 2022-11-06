// Fill out your copyright notice in the Description page of Project Settings.


#include "Haze/HazeObjects/HazeCableActor.h"

// Sets default values
AHazeCableActor::AHazeCableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	m_pHazeEffector = CreateDefaultSubobject<UHazeEffectComponent>(TEXT("Haze Component"));
	check(m_pHazeEffector != nullptr);

	
	m_pCable = CreateDefaultSubobject<UCableComponent>(TEXT("Cable Component"));
	check(m_pCable != nullptr);
}

void AHazeCableActor::OnHazeStart()
 {
	SetActorTickEnabled(true);
}

void AHazeCableActor::OnHazeFinish()
{
	SetActorTickEnabled(false);

	m_pCable->CableForce = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void AHazeCableActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	m_pHazeEffector->m_HazeComponentListeners.AddListener(this, "Light Listener");
}

// Called every frame
void AHazeCableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float forceInX = m_ForceMultiplierX * m_pHazeEffector->GetCurrentHazeModifier(0);
	const float forceInY = m_ForceMultiplierY * m_pHazeEffector->GetCurrentHazeModifier(1);

	const FVector totalForce = FVector(forceInX, forceInY, 0.0f);

	m_pCable->CableForce = totalForce;
}

