// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/EscapeActorComponent.h"

#include "Gameplay/TensionSubsystem.h"

class IInteractableInterface;

void UEscapeActorComponent::BeginPlay()
{
	Super::BeginPlay();
	UWorld* pWorld = GetWorld();
	if (!pWorld)
		return;

	UTensionSubsystem* pTension = pWorld->GetSubsystem<UTensionSubsystem>();
	if (!pTension)
		return;
	IInteractableInterface* interface = Cast<IInteractableInterface>(GetOwner());
	TScriptInterface<IInteractableInterface> scriptInterface;
	scriptInterface.SetInterface(interface);
	scriptInterface.SetObject(GetOwner());
	pTension->RegisterEscapeLocation(scriptInterface);
}

void UEscapeActorComponent::ActivateLocation()
{
	m_IsActive = true;
}

void UEscapeActorComponent::DeactivateLocation()
{
	m_IsActive = false;
}

bool UEscapeActorComponent::GetIsActive() const
{
	return m_IsActive;
}


