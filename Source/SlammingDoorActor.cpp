// Fill out your copyright notice in the Description page of Project Settings.


#include "SlammingDoorActor.h"
#include "HazeEffectComponent.h"
// Sets default values
ASlammingDoorActor::ASlammingDoorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pHazeEffectComponent = CreateDefaultSubobject<UHazeEffectComponent>(TEXT("Haze Component"));
	check(m_pHazeEffectComponent != nullptr);
}

// Called when the game starts or when spawned
void ASlammingDoorActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(true);
}

void ASlammingDoorActor::SetIsInSlammableState(bool set)
{
	if (m_bIsInSlammableState == set)
		return;

	m_bIsInSlammableState = set;
	SetActorTickEnabled(m_bIsInSlammableState);

	if (!m_bIsInSlammableState)
		OnStopSlammable();
}

void ASlammingDoorActor::OnSlamFinished()
{
	SetActorTickEnabled(true);
}


// Called every frame
void ASlammingDoorActor::Tick(float DeltaTime)
{
	if (m_bIsSlamming)
		return;
	
	Super::Tick(DeltaTime);
	const float currentModifier = FMath::Abs(m_pHazeEffectComponent->GetCurrentHazeModifier());
	const float currentStrength = m_pHazeEffectComponent->GetCurrentHazeStrength();
	const float slamProbability = m_SlamProbabilityByHazeStrength.EditorCurveData.Eval(currentStrength);
	const float slamSize = m_MaxSlamSizeByHazeModifier.EditorCurveData.Eval(currentModifier);
	const float rolledVal = FMath::FRandRange(0.0f, 1.0f);

	if (rolledVal > slamProbability)
		return;

	SetActorTickEnabled(false);
	OnBeginSlam(slamSize);
}

