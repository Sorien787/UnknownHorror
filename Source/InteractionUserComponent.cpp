// Fill out your copyright notice in the Description page of Project Settings.
#pragma optimize("", off)

#include "InteractionUserComponent.h"

#include "UnrealUtilities.h"

// Sets default values for this component's properties
UInteractionUserComponent::UInteractionUserComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UInteractionUserComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionUserComponent::TriggerInteraction()
{
	// currently not implemented - finish the interaction immediately
	m_pCurrentUsingInteractable->OnInteractionFinished(this);
}

bool UInteractionUserComponent::HasInteractionAnim(InteractableObjectType type) const
{
	return true;
}

IInteractableInterface* UInteractionUserComponent::ClosestInteractionQuery(bool ignoreCurrentInteractable) const
{
	float distanceSquared = m_fInteractionRange * m_fInteractionRange;
	float highestInteractionPriority = 0;
	FVector desiredLocation = GetOwner()->GetActorLocation();

	if (m_bIsPlayerInteractionUser)
		desiredLocation = UnrealUtilities::RaycastActorToWorldPosition(GetWorld(), m_fInteractionRange, GetOwner());
	
	IInteractableInterface* pClosestInteractable = nullptr;
	for(IInteractableInterface* pInteractable : m_InteractionCandidates)
	{
		if (ignoreCurrentInteractable && pInteractable == m_pCurrentUsingInteractable)
			continue;

		int interactionType = 0;
		if (!pInteractable->IsInteractionAvailable(this, interactionType))
			continue;

		const float distanceToInteractableSquared = (pInteractable->GetCurrentLocation() - desiredLocation).SizeSquared();
		const int interactionPriority = pInteractable->GetInteractionPriority();

		if (interactionPriority < highestInteractionPriority)
			continue;

		// only skip the distance check if this interaction's priority is higher than the highest so far.
		if (distanceToInteractableSquared >= distanceSquared && (interactionPriority < highestInteractionPriority))
			continue;
		
		highestInteractionPriority = interactionPriority;
		distanceSquared = distanceToInteractableSquared;
		pClosestInteractable = pInteractable;
	}
	return pClosestInteractable;
}

void UInteractionUserComponent::FocusedInteractionUpdate()
{
	// only ignore the closest interactable if the current using interactable is not null (since that one will be closest)
	IInteractableInterface* pClosestInteractable = ClosestInteractionQuery();
	
	if (pClosestInteractable == m_pCurrentFocusedInteractable)
		return;

	// if we had an interactable nearby and it's different, clear it
	if (m_pCurrentFocusedInteractable)
		ClearFocusedInteractable();

	// if we have an interactable nearby, set that as our usable one
	if (pClosestInteractable)
		SetNewFocusedInteractable(pClosestInteractable);
}

void UInteractionUserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FocusedInteractionUpdate();
}

void UInteractionUserComponent::ClearFocusedInteractable()
{
	if (m_bShouldTriggerWidgets)
		m_pCurrentFocusedInteractable->OnHideFocusedInteractionWidget();
	m_pCurrentFocusedInteractable = nullptr;
}

void UInteractionUserComponent::SetNewFocusedInteractable(IInteractableInterface* pNewInteractable)
{
	m_pCurrentFocusedInteractable = pNewInteractable;
	if (m_bShouldTriggerWidgets)
		m_pCurrentFocusedInteractable->OnShowFocusedInteractionWidget();
}

void UInteractionUserComponent::OnDisengageWithInteraction()
{
	m_pCurrentUsingInteractable = nullptr;
}

void UInteractionUserComponent::OnInteractButtonPressed()
{
	int interactionType = 0;
	if (!m_pCurrentFocusedInteractable || m_pCurrentFocusedInteractable->IsInteractionAvailable(this, OUT interactionType))
		return;
	OnInteractWithFocusedInteractable();
}


void UInteractionUserComponent::OnInteractWithFocusedInteractable()
{
	m_pCurrentUsingInteractable = m_pCurrentFocusedInteractable;
	m_pCurrentUsingInteractable->OnInteractionStarted(this);
	TriggerInteraction();
}

float UInteractionUserComponent::GetDropRange() const
{
	return m_DropRange;
}


/////////////////////////////////////
///// Box overlap functions
void UInteractionUserComponent::AddInteractionEnterBox(UBoxComponent* pBox)
{
	m_pEnterBox = pBox;
	m_pEnterBox->OnComponentBeginOverlap.AddDynamic(this, &UInteractionUserComponent::OnBoxBeginOverlap);
}

void UInteractionUserComponent::AddInteractionExitBox(UBoxComponent* pBox)
{
	m_pExitBox = pBox;
	m_pExitBox->OnComponentEndOverlap.AddDynamic(this, &UInteractionUserComponent::OnBoxEndOverlap);
}

void UInteractionUserComponent::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractableInterface* pInteractable = Cast<IInteractableInterface>(OtherActor);

	if (!pInteractable)
		return;

	m_InteractionCandidates.Add(pInteractable);

	pInteractable->OnShowInteractionWidget();
}


void UInteractionUserComponent::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractableInterface* pInteractable = Cast<IInteractableInterface>(OtherActor);

	if (!pInteractable)
		return;

	m_InteractionCandidates.Remove(pInteractable);

	pInteractable->OnHideInteractionWidget();
}
/////////////////////////////////////



