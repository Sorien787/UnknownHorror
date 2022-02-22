// Fill out your copyright notice in the Description page of Project Settings.


#include "LightInteractable.h"
#include "InteractionUserComponent.h"

ALightInteractable::ALightInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	m_LightSwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	check(m_LightSwitchMesh != nullptr);
	m_LightSwitchMesh->SetupAttachment(m_RootComponent);

	m_Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light To Switch"));
	check(m_Light != nullptr);
	m_Light->SetupAttachment(m_RootComponent);

	m_LightModifier = CreateDefaultSubobject<ULightModifierComponent>(TEXT("Light Modifier"));
	check(m_LightModifier != nullptr);
}

void ALightInteractable::BeginPlay()
{
	Super::BeginPlay();

	m_LightModifier->AddLightToControlGroup(m_Light);
	SetLightStateBasedOnData();
}

void ALightInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALightInteractable::SetLightStateBasedOnData()
{
	if (m_IsSwitchOn)
	{
		m_LightModifier->SwitchOn();
	}
	else
	{
		m_LightModifier->SwitchOff();
	}
}


void ALightInteractable::OnInteractionStarted(UInteractionUserComponent* pInteractable) 
{
	m_IsSwitchOn = !m_IsSwitchOn;
	SetLightStateBasedOnData();
	pInteractable->OnDisengageWithInteraction();
}

bool ALightInteractable::IsInteractionAvailable(const UInteractionUserComponent* pInteractable, OUT int type) const
{
	return true;
}
