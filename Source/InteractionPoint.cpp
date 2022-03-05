// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionPoint.h"
#include "UnrealUtilities.h"

// Sets default values
AInteractionPoint::AInteractionPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pInteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget"));
	check(m_pInteractWidget != nullptr);
	m_pInteractWidget->SetupAttachment(GetRootComponent());

	m_TriggerBoxComponent= CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Trigger Box"));
	check(m_TriggerBoxComponent != nullptr);
	m_TriggerBoxComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AInteractionPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractionPoint::TryRevealWidget()
{
	if (m_CurrentWidgetState != CurrentWidgetState::Hidden)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Revealed;
	InteractionWidgetReveal();
}

void AInteractionPoint::TryHideWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Hidden)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Hidden;
	InteractionWidgetHide();
}

void AInteractionPoint::TryFocusWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Interactable)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Interactable;
	InteractionWidgetFocus();
}

void AInteractionPoint::TryUnfocusWidget()
{
	if (m_CurrentWidgetState != CurrentWidgetState::Interactable)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Revealed;
	InteractionWidgetFocus();
}

FVector AInteractionPoint::GetCurrentLocation() const
{
	return GetActorLocation();
}

void AInteractionPoint::Tick(float DeltaSeconds)
{
	m_pInteractWidget->SetWorldRotation(UnrealUtilities::GetRotationMatrixToPlayer(GetWorld(), GetCurrentLocation()));
}

void AInteractionPoint::RegisterParent(IInteractableInterface* pInteractableInterface)
{
	m_pInteractableInterface = pInteractableInterface;
}

void AInteractionPoint::TryInteract(UInteractionUserComponent* pUser)
{
	m_pInteractableInterface->OnInteractionStarted(pUser);
	if (m_pInteractableInterface->IsFastInteraction())
	{
		InteractionWidgetInteractFast();
		return;
	}
	InteractionWidgetInteractSlow();
	m_CurrentWidgetState = CurrentWidgetState::Hidden;
}

bool AInteractionPoint::HasLinkedInteractable() const
{
	return m_pInteractableInterface != nullptr;
}
