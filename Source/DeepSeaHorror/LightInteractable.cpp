// Fill out your copyright notice in the Description page of Project Settings.


#include "LightInteractable.h"
#include "InteractionUserComponent.h"

ALightInteractable::ALightInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = m_RootComponent;
	check(m_RootComponent != nullptr);

	m_LightSwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	check(m_LightSwitchMesh != nullptr);
	m_LightSwitchMesh->SetupAttachment(m_RootComponent);

	m_Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light To Switch"));
	check(m_Light != nullptr);
	m_Light->SetupAttachment(m_RootComponent);
	
	m_pInteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget"));
	check(m_pInteractionWidget != nullptr);
	m_pInteractionWidget->SetupAttachment(m_RootComponent);

	m_LightModifier = CreateDefaultSubobject<ULightModifierComponent>(TEXT("Light Modifier"));
	check(m_LightModifier != nullptr);
}

void ALightInteractable::BeginPlay()
{
	Super::BeginPlay();

	SetLightStateBasedOnData();
}

void ALightInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ALightInteractable::GetCurrentLocation() const
{
	return GetActorLocation();
}

void ALightInteractable::SetLightStateBasedOnData()
{
	if (m_IsSwitchOn)
	{
		m_Light->SetIntensity(m_LightOnIntensity);
	}
	else
	{
		m_Light->SetIntensity(0);
	}
}


void ALightInteractable::OnInteract(UInteractionUserComponent* pInteractable) 
{
	m_IsSwitchOn = !m_IsSwitchOn;
	SetLightStateBasedOnData();
	pInteractable->OnDisengageWithInteraction();
}

bool ALightInteractable::IsInteractionAvailable(UInteractionUserComponent* pInteractable) const
{
	return true;
}

void ALightInteractable::OnShowInteractionWidget() 
{

}

void ALightInteractable::OnHideInteractionWidget() 
{

}

