// Fill out your copyright notice in the Description page of Project Settings.
#pragma optimize("", off)

#include "InteractionUserComponent.h"

#include "InteractableObject.h"
#include "../Common/UnrealUtilities.h"

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

void UInteractionUserComponent::SetInteractionUserType(InteractionUserType userType)
{
	m_UserType = userType;
}

IInteractionTriggerInterface* UInteractionUserComponent::ClosestInteractionQuery(bool ignoreCurrentInteractable) const
{
	if (!m_bIsPlayerInteractionUser)
		return nullptr;
	
	TArray<FHitResult> hits = UnrealUtilities::RaycastActorToWorldHit(GetWorld(), m_fInteractionRange, GetOwner());

	IInteractionTriggerInterface* pClosestObject = nullptr;
	float closestInteractableDistance = FLT_MAX;
	for (auto& hit : hits)
	{
		IInteractionTriggerInterface* pInteractableObject = nullptr;
		if (!hit.GetActor())
			continue;
		
		pInteractableObject = Cast<IInteractionTriggerInterface>(hit.GetActor());
		
		if (!pInteractableObject || !pInteractableObject->GetCanInteract(this) || hit.Distance > closestInteractableDistance)
			continue;

		closestInteractableDistance = hit.Distance;
		pClosestObject = pInteractableObject;
		break;
	}

	if (pClosestObject)
		return pClosestObject;
	
	closestInteractableDistance = FLT_MAX;
		
	for (auto& interactable : m_InteractionCandidates)
	{
		if (!interactable->GetIsForcedFocus() || !interactable->GetCanInteract(this))
			continue;
	
		const float temp_interactableDistance = FVector::DistSquared(interactable->GetInteractorTransform().GetLocation(), GetOwner()->GetTransform().GetLocation());

		if (temp_interactableDistance > closestInteractableDistance)
			continue;
	
		closestInteractableDistance = temp_interactableDistance;
		pClosestObject = interactable;
	}
	if(!pClosestObject || !pClosestObject->GetHasLinkedInteractable() || !pClosestObject->GetIsEnabled() || !pClosestObject->GetCanInteract(this))
		return nullptr;

	return pClosestObject;
}

void UInteractionUserComponent::FocusedInteractionUpdate()
{
	// only ignore the closest interactable if the current using interactable is not null (since that one will be closest)
	IInteractionTriggerInterface* pClosestInteractable = ClosestInteractionQuery();

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
		if (!element->GetCanInteract(this))
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

void UInteractionUserComponent::SetNewFocusedInteractable(IInteractionTriggerInterface* pNewInteractable)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("New Focused interaction"));
	m_pCurrentFocusedInteractionPoint = pNewInteractable;
	if (m_bShouldTriggerWidgets)
		m_pCurrentFocusedInteractionPoint->TryFocusWidget();
}


void UInteractionUserComponent::OnInteractionFinished()
{
	m_bIsCurrentlyInInteractionAnimation = false;
	EnableInteractions();
}

void UInteractionUserComponent::OnInteractButtonPressed()
{
	if (!m_pCurrentFocusedInteractionPoint || !m_bInteractionsEnabled)
		return;

	OnInteractWithFocusedInteractable();
}

bool UInteractionUserComponent::IsCurrentlyInInteractionAnimation() const
{
	return m_bIsCurrentlyInInteractionAnimation;
}

float UInteractionUserComponent::GetAnimCameraYaw() const
{
	if (!m_pCurrentUsingInteractionPoint)
		return m_DefaultCameraYawTolerance;
	return m_pCurrentUsingInteractionPoint->GetCameraYawTolerance();
}

float UInteractionUserComponent::GetAnimCameraPitch() const
{
	if (!m_pCurrentUsingInteractionPoint)
		return m_DefaultCameraPitchTolerance;
	return m_pCurrentUsingInteractionPoint->GetCameraPitchTolerance();
}

void UInteractionUserComponent::OnInteractWithFocusedInteractable()
{
	
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Interact with focused interactable"));
	m_pCurrentUsingInteractionPoint = m_pCurrentFocusedInteractionPoint;
	m_pCurrentUsingInteractionPoint->TryInteract(this);
	if (!m_pCurrentUsingInteractionPoint->GetIsFastInteractable())
	{
		m_bIsCurrentlyInInteractionAnimation = true;
		DisableInteractions();
	}
	m_pCurrentFocusedInteractionPoint = nullptr;
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

const float UInteractionUserComponent::GetInteractionRange() const
{
	return m_fInteractionRange;
}

void UInteractionUserComponent::EnableInteractions()
{
	if (m_bInteractionsEnabled)
		return;
	m_bInteractionsEnabled = true;
	for (auto& it : m_InteractionCandidates)
    {
		if (!it->GetCanInteract(this))
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

	if (!m_bInteractionsEnabled || !pInteractable->GetCanInteract(this))
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



