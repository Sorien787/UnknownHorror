// Fill out your copyright notice in the Description page of Project Settings.
#include "InteractionUserComponent.h"

#include "InteractionPoint.h"
#include "../Common/UnrealUtilities.h"

static TAutoConsoleVariable<int32> InteractionUserDebug(
	TEXT("InteractionUserDebug"),
	0,
	TEXT("InteractionUserDebug.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);


// Sets default values for this component's properties
UInteractionPlayerComponent::UInteractionPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UInteractionPlayerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionPlayerComponent::SetInteractionUserType(InteractionUserType userType)
{
	m_UserType = userType;
}

IInteractionTriggerInterface* UInteractionPlayerComponent::ClosestInteractionQuery(bool ignoreCurrentInteractable)
{
	TArray<FHitResult> hits = UnrealUtilities::RaycastActorToWorldHit(GetWorld(), m_fInteractionRange, GetOwner(), ECollisionChannel::ECC_GameTraceChannel1);

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

void UInteractionPlayerComponent::FocusedInteractionUpdate()
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

void UInteractionPlayerComponent::RevealInteractionUpdate()
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

void UInteractionPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FocusedInteractionUpdate();
	
	RevealInteractionUpdate();
}

void UInteractionPlayerComponent::OnInteractionStarted(const TScriptInterface<IInteractableInterface> interf)
{
	UInteractionComponentBase::OnInteractionStarted(interf);
	DisableInteractions();
}

void UInteractionPlayerComponent::OnInteractionFinished(const TScriptInterface<IInteractableInterface> interf)
{
	UInteractionComponentBase::OnInteractionFinished(interf);
	EnableInteractions();
}

void UInteractionPlayerComponent::ClearFocusedInteractable()
{
	if(InteractionUserDebug.GetValueOnGameThread())
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Focused interaction cleared"));
	m_pCurrentFocusedInteractionPoint->TryUnfocusWidget();
	m_pCurrentFocusedInteractionPoint = nullptr;
}

void UInteractionPlayerComponent::SetNewFocusedInteractable(IInteractionTriggerInterface* pNewInteractable)
{
	if(InteractionUserDebug.GetValueOnGameThread())
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("New Focused interaction"));
	m_pCurrentFocusedInteractionPoint = pNewInteractable;
	m_pCurrentFocusedInteractionPoint->TryFocusWidget();
}

void UInteractionPlayerComponent::OnInteractButtonPressed()
{
	if (!m_pCurrentFocusedInteractionPoint || !m_bInteractionsEnabled)
		return;

	OnInteractWithFocusedInteractable();
}

float UInteractionPlayerComponent::GetAnimCameraYaw() const
{
	if (!m_pCurrentUsingInteractionPoint)
		return m_DefaultCameraYawTolerance;
	return m_pCurrentUsingInteractionPoint->GetCameraYawTolerance();
}

float UInteractionPlayerComponent::GetAnimCameraPitch() const
{
	if (!m_pCurrentUsingInteractionPoint)
		return m_DefaultCameraPitchTolerance;
	return m_pCurrentUsingInteractionPoint->GetCameraPitchTolerance();
}

void UInteractionPlayerComponent::OnInteractWithFocusedInteractable()
{
	if(InteractionUserDebug.GetValueOnGameThread())
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Interact with focused interactable"));
	m_pCurrentUsingInteractionPoint = m_pCurrentFocusedInteractionPoint;
	m_pCurrentUsingInteractionPoint->TryInteract(this);
}

void UInteractionPlayerComponent::DisableInteractions()
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

const float UInteractionPlayerComponent::GetInteractionRange() const
{
	return m_fInteractionRange;
}

void UInteractionPlayerComponent::EnableInteractions()
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
void UInteractionPlayerComponent::AddInteractionEnterShape(UShapeComponent* pBox)
{
	m_pEnterShape = pBox;
	m_pEnterShape->OnComponentBeginOverlap.AddDynamic(this, &UInteractionPlayerComponent::OnBoxBeginOverlap);
}

void UInteractionPlayerComponent::AddInteractionExitShape(UShapeComponent* pBox)
{
	m_pExitShape = pBox;
	m_pExitShape->OnComponentEndOverlap.AddDynamic(this, &UInteractionPlayerComponent::OnBoxEndOverlap);
}

void UInteractionPlayerComponent::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AInteractionPoint* pInteractable = Cast<AInteractionPoint>(OtherActor);

	if (!pInteractable || m_InteractionCandidates.Contains(pInteractable))
		return;
	m_InteractionCandidates.Add(pInteractable);

	if (!m_bInteractionsEnabled || !pInteractable->GetCanInteract(this))
		return;

	pInteractable->TryRevealWidget();
}


void UInteractionPlayerComponent::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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



