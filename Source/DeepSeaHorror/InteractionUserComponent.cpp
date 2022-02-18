// Fill out your copyright notice in the Description page of Project Settings.
#pragma optimize("", off)

#include "InteractionUserComponent.h"

// Sets default values for this component's properties
UInteractionUserComponent::UInteractionUserComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UInteractionUserComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}
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

void UInteractionUserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FVector parentLocation = GetOwner()->GetActorLocation();
	float distance = FLT_MAX;

	IInteractableInterface* pClosestInteractable = nullptr;
	for(IInteractableInterface* pInteractable : m_InteractionCandidates)
	{
		if (!pInteractable->IsInteractionAvailable(this))
			continue;

		float distanceToInteractable = (pInteractable->GetCurrentLocation() - parentLocation).SizeSquared();

		if (distanceToInteractable >= distance)
			continue;

		distance = distanceToInteractable;
		pClosestInteractable = pInteractable;
	}

	if (pClosestInteractable == m_pCurrentFocusedInteractable)
		return;

	// if we had an interactable nearby and it's different, clear it
	if (m_pCurrentFocusedInteractable)
		ClearCurrentInteractable();

	// if we have an interactable nearby, set that as our usable one
	if (pClosestInteractable)
		SetNewInteractable(pClosestInteractable);

	// if we have an interactable, and we have the USE response...
	if (!m_bCurrentFocusedInteractableInUse && m_pCurrentFocusedInteractable && m_bInteractionsEnabled)
	{
		OnInteractWithFocusedInteractable();
	}
}

void UInteractionUserComponent::ClearCurrentInteractable()
{
	if (m_bShouldTriggerWidgets)
		m_pCurrentFocusedInteractable->OnHideInteractionWidget();
	m_pCurrentFocusedInteractable = nullptr;

}

bool UInteractionUserComponent::IsActorInView(AActor* pActor) const
{
	return true;
}

void UInteractionUserComponent::RaycastToInteractable()
{
	FVector playerViewPointLocation;
	FRotator playerRotationInformation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerViewPointLocation, OUT playerRotationInformation);

	float raycastRange = 100.0f;
	FVector lineTraceEnd = playerViewPointLocation + playerRotationInformation.Vector() * raycastRange;

	FHitResult hit;
	// false to ignore complex collisions
	FCollisionQueryParams traceParams(FName(TEXT("Raycast To Interactable")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT hit,
		playerViewPointLocation,
		lineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		traceParams);

	AActor* pActorHit = hit.GetActor();
}

void UInteractionUserComponent::SetNewInteractable(IInteractableInterface* pNewInteractable)
{
	m_pCurrentFocusedInteractable = pNewInteractable;
	if (m_bShouldTriggerWidgets)
		m_pCurrentFocusedInteractable->OnShowInteractionWidget();
}

void UInteractionUserComponent::OnDisengageWithInteraction()
{
	m_bCurrentFocusedInteractableInUse = false;
}

void UInteractionUserComponent::OnInteractWithFocusedInteractable()
{
	m_bCurrentFocusedInteractableInUse = true;
	m_pCurrentFocusedInteractable->OnInteract(this);

	// the interaction widget might remove itself from being used here if it's a one-and-done type: so dont hide interaction widgets if it's not in use by this point
	if (m_bShouldTriggerWidgets && m_bCurrentFocusedInteractableInUse)
		m_pCurrentFocusedInteractable->OnHideInteractionWidget();

}

void UInteractionUserComponent::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractableInterface* pInteractable = Cast<IInteractableInterface>(OtherActor);

	if (!pInteractable)
		return;

	m_InteractionCandidates.Add(pInteractable);
}


void UInteractionUserComponent::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractableInterface* pInteractable = Cast<IInteractableInterface>(OtherActor);

	if (!pInteractable)
		return;

	m_InteractionCandidates.Remove(pInteractable);
}

void UInteractionUserComponent::OnInteractButtonPressed()
{
	if (m_bCurrentFocusedInteractableInUse)
		return;

	if (m_InteractionCandidates.Num() == 0)
		return;
	if (!m_pCurrentFocusedInteractable)
		return;
	OnInteractWithFocusedInteractable();
}



