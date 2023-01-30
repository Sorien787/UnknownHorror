// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/LurkInteractableComponent.h"

#include "Gameplay/TensionSubsystem.h"

void ULurkInteractableComponent::BeginPlay()
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
	pTension->RegisterLurkLocation(scriptInterface);
}

void ULurkInteractableComponent::ActivateLocation()
{
	m_IsActive = true;
}

void ULurkInteractableComponent::DeactivateLocation()
{
	m_IsActive = false;
}

bool ULurkInteractableComponent::GetIsActive() const
{
	return m_IsActive;
}


