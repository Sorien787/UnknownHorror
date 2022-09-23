// Fill out your copyright notice in the Description page of Project Settings.


#include "PressureGaugeActor.h"


// Sets default values
APressureGaugeActor::APressureGaugeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	m_pHazeEffector = CreateDefaultSubobject<UHazeEffectComponent>(TEXT("Haze Component"));
	check(m_pHazeEffector != nullptr);

	m_HazeStrengthToAcceleration.EditorCurveData.AddKey(0.0f, 1.0f);
	m_HazeStrengthToAcceleration.EditorCurveData.AddKey(1.0f, 1.0f);
}

// Called when the game starts or when spawned
void APressureGaugeActor::BeginPlay()
{
	Super::BeginPlay();
	
	m_fCurrentAngle = m_fDefaultAngle;
}

// Called every frame
void APressureGaugeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float acceleration = 0.0f;
	
	const float distanceFromDefault = m_fCurrentAngle - m_fDefaultAngle;
	acceleration -= distanceFromDefault * m_SpringContant + m_fCurrentVelocity * 2 * m_SpringDamping;
	if (abs(m_fCurrentAngle) < m_fMaxAngle || abs(m_fCurrentVelocity) < m_VelocityForUncontrolledSpin)
	{
		if (m_pHazeEffector && m_pHazeEffector->IsHazeActive())
		{
			const float currentHazeStrength = m_pHazeEffector->GetCurrentHazeStrength();
			const float currentStrengthChange = m_HazeStrengthToAcceleration.EditorCurveData.Eval(currentHazeStrength);
			acceleration += m_pHazeEffector->GetCurrentHazeModifier() * currentStrengthChange;
		}
	}
	m_fCurrentVelocity += acceleration * DeltaTime;

	m_fCurrentAngle += m_fCurrentVelocity * DeltaTime;

	if (m_fCurrentAngle > 180.0f)
	{
		m_fCurrentAngle -= 360.0f;		
	}
	else if (m_fCurrentAngle < -180.0f)
	{
		m_fCurrentAngle += 360.0f;
	}

	OnSetPressureGaugeAngle(m_fCurrentAngle);
}

