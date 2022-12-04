// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIAlertLevel.h"

#include "PerceptionTypeComponent.h"

EAIAlertLevel FActorAlertData::GetMaximumAlertLevel() const
{
	return pPerceptionComponent->GetMaximumAlertLevel();
}

float FActorAlertData::GetMaximumPerceptionStrength() const
{
	return pPerceptionComponent->GetMaximumInterest();
}
