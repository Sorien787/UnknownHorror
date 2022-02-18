// Fill out your copyright notice in the Description page of Project Settings.


#include "LightModifierComponent.h"

// Sets default values for this component's properties
ULightModifierComponent::ULightModifierComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void ULightModifierComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULightModifierComponent::AddLightToControlGroup(ULightComponent* pLightComponent)
{
	m_pLightComponentsArray.Add(pLightComponent);
}

// Called every frame
void ULightModifierComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

