
#include "InteractableObjectBase.h"

#include "UnrealUtilities.h"

AInteractableObjectBase::AInteractableObjectBase()
{
	m_pRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	check(m_pRootComponent != nullptr);
	RootComponent = m_pRootComponent;

	m_pInteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget"));
	check(m_pInteractWidget != nullptr);
	m_pInteractWidget->SetupAttachment(GetRootComponent());

	m_TriggerBoxComponent= CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Trigger Box"));
	check(m_TriggerBoxComponent != nullptr);
	m_TriggerBoxComponent->SetupAttachment(GetRootComponent());
}

void AInteractableObjectBase::TryRevealWidget()
{
	if (m_CurrentWidgetState != CurrentWidgetState::Hidden)
		return;
	RevealWidget();
}

void AInteractableObjectBase::TryHideWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Hidden)
		return;
	HideWidget();
}

void AInteractableObjectBase::TryFocusWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Interactable)
		return;
	ShowInteraction();
}

void AInteractableObjectBase::TryUnfocusWidget()
{
	if (m_CurrentWidgetState != CurrentWidgetState::Interactable)
		return;
	HideInteraction();
}

int AInteractableObjectBase::GetInteractionPriority()
{
	return m_InteractionPriority;
}

FVector AInteractableObjectBase::GetCurrentLocation() const
{
	return GetActorLocation();
}

void AInteractableObjectBase::Tick(float DeltaSeconds)
{
	m_pInteractWidget->SetWorldRotation(UnrealUtilities::GetRotationMatrixToPlayer(GetWorld(), GetCurrentLocation()));
}

void AInteractableObjectBase::OnInteractionStarted(UInteractionUserComponent* pInteractionUser)
{
	if (m_bIsQuickInteraction)
	{
		OnInstantInteract();
		return;
	}
	OnAnimInteract();
	m_CurrentWidgetState = CurrentWidgetState::Hidden;
}


