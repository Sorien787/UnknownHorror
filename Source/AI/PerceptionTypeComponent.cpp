// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PerceptionTypeComponent.h"

#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
// Sets default values for this component's properties
UPerceptionTypeComponent::UPerceptionTypeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


uint8 UPerceptionTypeComponent::GetPriority() const
{
	return m_Priority;
}

bool UPerceptionTypeComponent::GetCanBeIgnored() const
{
	return m_CanBeIgnored;
}

const FPerceptionTypeModifier* UPerceptionTypeComponent::GetPerceptionTypeModifierForSense(FAISenseID senseID) const
{
	// convert sense ID to sense enum
	if (senseID == UAISense::GetSenseID<UAISense_Sight>())
	{
		return m_PerceptionTypeMap.Find(ESenseEnum::Sight);
	}
	if (senseID == UAISense::GetSenseID<UAISense_Hearing>())
	{
		return m_PerceptionTypeMap.Find(ESenseEnum::Hearing);
	}
	return m_PerceptionTypeMap.Find(ESenseEnum::Default);
}

