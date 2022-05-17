// Fill out your copyright notice in the Description page of Project Settings.


#include "DualStateInteractable.h"

// Sets default values
ADualStateInteractable::ADualStateInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
}

// Called when the game starts or when spawned
void ADualStateInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADualStateInteractable::OnInteractionStarted(UInteractionUserComponent* pInteractionUser, int interactorId)
{
	Super::OnInteractionStarted(pInteractionUser, interactorId);
	// for now, immediately finish the interaction - we'll sort out the proper animation delay later.
}

bool ADualStateInteractable::IsInteractionAvailable(const UInteractionUserComponent* pInteractionUser, int interactorId)
{
	return Super::IsInteractionAvailable(pInteractionUser, interactorId);
}

void ADualStateInteractable::OnInteractionFinished(UInteractionUserComponent* pInteractionUser)
{
	Super::OnInteractionFinished(pInteractionUser);
	if (m_CurrentState == DualStateObjectState::Closed)
		m_CurrentState = DualStateObjectState::Open;
	else
		m_CurrentState = DualStateObjectState::Closed;
	OnChangeState(m_CurrentState);
}




