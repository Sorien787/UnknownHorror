// Fill out your copyright notice in the Description page of Project Settings.


#include "LightHazeActor.h"

// Sets default values
ALightHazeActor::ALightHazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetActorTickEnabled(false);

	m_pHazeEffectComponent = CreateDefaultSubobject<UHazeEffectComponent>(TEXT("Haze Component"));
	check(m_pHazeEffectComponent != nullptr);

	m_pLightModifier = CreateDefaultSubobject<ULightModifierComponent>(TEXT("Light Modifier Component"));
	check(m_pLightModifier != nullptr);
}

// Called when the game starts or when spawned
void ALightHazeActor::BeginPlay()
{
	Super::BeginPlay();
	m_pHazeEffectComponent->m_HazeComponentListeners.AddListener(this, "Light Listener");
}

void ALightHazeActor::OnHazeEvent()
{
	m_pLightModifier->Break();
	m_pHazeEffectComponent->m_HazeComponentListeners.RemoveListener(this);
}

void ALightHazeActor::OnHazeFinish()
{
	SetActorTickEnabled(false);
	
	m_pLightModifier->CancelFlickerStatusOverride();
}

void ALightHazeActor::OnHazeStart()
{
	SetActorTickEnabled(true);
}

void ALightHazeActor::Tick(float DeltaSeconds)
{
	const float value = m_pHazeEffectComponent->GetCurrentHazeStrength();
	
	m_pLightModifier->SetFlickerStatusOverride(FLightFlickerStateStruct(
    m_LightMinBrightness.EditorCurveData.Eval(value),
    m_LightMaxBrightness.EditorCurveData.Eval(value),
    m_LightFlickerFrequencyByHazeModifier.EditorCurveData.Eval(value),
    m_LightPercentOnlineByHazeModifier.EditorCurveData.Eval(value)
    ));
}

