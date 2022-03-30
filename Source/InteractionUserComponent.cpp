// Fill out your copyright notice in the Description page of Project Settings.
#pragma optimize("", off)

#include "InteractionUserComponent.h"

#include "InteractableObject.h"
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

AInteractionPoint* UInteractionUserComponent::ClosestInteractionQuery(bool ignoreCurrentInteractable) const
{
	if (!m_bIsPlayerInteractionUser)
		return nullptr;
	
	FHitResult hit = UnrealUtilities::RaycastActorToWorldHit(GetWorld(), m_fInteractionRange, GetOwner());

	AInteractionPoint* pInteractableObject = nullptr;

	if (hit.GetActor())
	{
		pInteractableObject = Cast<AInteractionPoint>(hit.GetActor());
	}
	
	if (!pInteractableObject)
	{
		float closestInteractableDistance = FLT_MAX;
		for (auto& interactable : m_InteractionCandidates)
		{
			if (!interactable->ForceFocus() || !interactable->CanInteract(this))
				continue;
			
			const float temp_interactableDistance = FVector::DistSquared(interactable->GetActorTransform().GetLocation(), GetOwner()->GetTransform().GetLocation());

			if (temp_interactableDistance > closestInteractableDistance)
				continue;
			
			closestInteractableDistance = temp_interactableDistance;
			pInteractableObject = interactable;
		}
	}
	
	if(!pInteractableObject || !pInteractableObject->HasLinkedInteractable() || !pInteractableObject->GetIsEnabled() || !pInteractableObject->CanInteract(this))
		return nullptr;

	return pInteractableObject;
}

void UInteractionUserComponent::FocusedInteractionUpdate()
{
	// only ignore the closest interactable if the current using interactable is not null (since that one will be closest)
	AInteractionPoint* pClosestInteractable = ClosestInteractionQuery();

	if (!m_InteractionCandidates.Contains(pClosestInteractable))
		pClosestInteractable = nullptr;
	
	if (pClosestInteractable == m_pCurrentFocusedInteractionPoint)
		return;

	// if we had an interactable nearby and it's different, clear it
	if (m_pCurrentFocusedInteractionPoint)
		ClearFocusedInteractable();

	// if we have an interactable nearby, set that as our usable one
	if (pClosestInteractable)
		SetNewFocusedInteractable(pClosestInteractable);
}

void UInteractionUserComponent::RevealInteractionUpdate()
{
	for (const auto& element : m_InteractionCandidates)
	{
		if (!element->CanInteract(this))
		{
			element->TryHideWidget();
			continue;
		}
		element->TryRevealWidget();
	}
}

void UInteractionUserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FocusedInteractionUpdate();
	
	RevealInteractionUpdate();
}

void UInteractionUserComponent::ClearFocusedInteractable()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Focused interaction cleared"));
	if (m_bShouldTriggerWidgets)
		m_pCurrentFocusedInteractionPoint->TryUnfocusWidget();
	m_pCurrentFocusedInteractionPoint = nullptr;
}

void UInteractionUserComponent::SetNewFocusedInteractable(AInteractionPoint* pNewInteractable)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("New Focused interaction"));
	m_pCurrentFocusedInteractionPoint = pNewInteractable;
	if (m_bShouldTriggerWidgets)
		m_pCurrentFocusedInteractionPoint->TryFocusWidget();
}


void UInteractionUserComponent::OnInteractionFinished()
{
	EnableInteractions();
}

void UInteractionUserComponent::OnInteractButtonPressed()
{
	if (!m_pCurrentFocusedInteractionPoint || !m_bInteractionsEnabled)
		return;

	OnInteractWithFocusedInteractable();
}


void UInteractionUserComponent::OnInteractWithFocusedInteractable()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Interact with focused interactable"));
	m_pCurrentUsingInteractionPoint = m_pCurrentFocusedInteractionPoint;
	m_pCurrentUsingInteractionPoint->TryInteract(this);
	if (!m_pCurrentUsingInteractionPoint->IsFastInteractable())
		DisableInteractions();
}

void UInteractionUserComponent::DisableInteractions()
{
	if (!m_bInteractionsEnabled)
		return;
	m_bInteractionsEnabled = false;
	for (auto it = m_InteractionCandidates.CreateIterator(); it; ++it)
	{
		(*it)->TryHideWidget();
	}
	PrimaryComponentTick.SetTickFunctionEnable(false);
}

bool UInteractionUserComponent::IsHandFull() const
{
	return m_pObjectInHand != nullptr;
}

const float UInteractionUserComponent::GetInteractionRange() const
{
	return m_fInteractionRange;
}


bool UInteractionUserComponent::AddObjectToHand(AInteractableObject* pInteractableObject)
{
	pInteractableObject->AttachToActor(GetOwner(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("object_handAttachmentPoint"));
	m_pObjectInHand = pInteractableObject;
	return true;
}

AInteractableObject* UInteractionUserComponent::GetCurrentInObjectInHand() const
{
	return m_pObjectInHand;
}

AInteractableObject* UInteractionUserComponent::RemoveCurrentObjectInHand()
{
	
	AInteractableObject* temp = std::move(m_pObjectInHand);
	temp->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	m_pObjectInHand = nullptr;
	return temp;
}

void UInteractionUserComponent::EnableInteractions()
{
	if (m_bInteractionsEnabled)
		return;
	m_bInteractionsEnabled = true;
	for (auto& it : m_InteractionCandidates)
    {
		if (!it->CanInteract(this))
			continue;
    	it->TryRevealWidget();
    }
	PrimaryComponentTick.SetTickFunctionEnable(true);
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
	AInteractionPoint* pInteractable = Cast<AInteractionPoint>(OtherActor);

	if (!pInteractable || m_InteractionCandidates.Contains(pInteractable))
		return;
	m_InteractionCandidates.Add(pInteractable);

	if (!m_bInteractionsEnabled || !pInteractable->CanInteract(this))
		return;

	pInteractable->TryRevealWidget();
}


void UInteractionUserComponent::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AInteractionPoint* pInteractable = Cast<AInteractionPoint>(OtherActor);
	if (!pInteractable || !m_InteractionCandidates.Contains(pInteractable))
		return;
	
	m_InteractionCandidates.Remove(pInteractable);

	if (!m_bInteractionsEnabled || !pInteractable->GetIsEnabled())
		return;
	pInteractable->TryHideWidget();
}
/////////////////////////////////////



