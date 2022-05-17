// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionPoint.h"

#include "InteractionUserComponent.h"
#include "UnrealUtilities.h"
#include "Kismet/KismetMathLibrary.h"
#pragma optimize("", off)
// Sets default values
AInteractionPoint::AInteractionPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	check(m_RootComponent != nullptr);
	SetRootComponent(m_RootComponent);

	m_WidgetAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Widget Attachment Point"));
	check(m_WidgetAttachmentPoint != nullptr);
	m_WidgetAttachmentPoint->SetupAttachment(GetRootComponent());

	m_pInteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget"));
	check(m_pInteractWidget != nullptr);
	m_pInteractWidget->SetupAttachment(m_WidgetAttachmentPoint);

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
	if (m_CurrentWidgetState != CurrentWidgetState::Hidden || !m_bIsCurrentlyActive)
		return;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Revealing Widget!"));
	m_CurrentWidgetState = CurrentWidgetState::Revealed;
	Execute_InteractionWidgetReveal(this);
}

void AInteractionPoint::TryHideWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Hidden)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Hidden;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Hiding Widget!"));
	Execute_InteractionWidgetHide(this);
}

void AInteractionPoint::TryFocusWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Interactable || !m_bIsCurrentlyActive)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Interactable;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Focusing Widget!"));
	Execute_InteractionWidgetFocus(this);
}

void AInteractionPoint::TryUnfocusWidget()
{
	if (m_CurrentWidgetState != CurrentWidgetState::Interactable || !m_bIsCurrentlyActive)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Revealed;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Unfocusing Widget!"));
	Execute_InteractionWidgetUnfocus(this);
}

FVector AInteractionPoint::GetCurrentLocation() const
{
	return GetActorLocation();
}

bool AInteractionPoint::IsActorInRangeToInteract(FVector position) const
{
	const FTransform& actorTransform = GetActorTransform();
	const FVector interactionPointForwardVector = actorTransform.GetUnitAxis(EAxis::X);
	const FVector interactionPointPosition = actorTransform.GetLocation();

	FVector interactionPoint_to_otherActor = position - interactionPointPosition;
	if (!interactionPoint_to_otherActor.Normalize())
		return true;

	const float angleBetweenForwardAndActor = FMath::Acos(FVector::DotProduct(interactionPoint_to_otherActor, interactionPointForwardVector));

	return FMath::RadiansToDegrees(angleBetweenForwardAndActor) < m_TriggerAngle;
}

int AInteractionPoint::GetInteractorId() const
{
	return m_interactorId;
}

bool AInteractionPoint::GetIsEnabled() const
{
	return m_bIsCurrentlyActive;
}

void AInteractionPoint::SetIsEnabled(bool enabled)
{
	m_bIsCurrentlyActive = enabled;
	SetActorEnableCollision(enabled);
	if (!m_bIsCurrentlyActive && m_CurrentWidgetState != CurrentWidgetState::Hidden)
		TryHideWidget();
}

bool AInteractionPoint::CanInteract(const UInteractionUserComponent* pUser) const
{
	if (!m_bIsCurrentlyActive)
		return false;

	if (!m_pInteractableInterface)
	{
		__debugbreak();
		return false;
	}
	
	if (!IsActorInRangeToInteract(pUser->GetOwner()->GetActorLocation()))
		return false;
	
	return m_pInteractableInterface->IsInteractionAvailable(pUser, m_interactorId);
}

void AInteractionPoint::Tick(float DeltaSeconds)
{
	const FTransform playerCameraTransform = UnrealUtilities::GetPlayerCameraTransform(GetWorld());
	const FQuat rotatorQuat = FQuat(playerCameraTransform.GetUnitAxis(EAxis::Z), PI);
	const FQuat iconRotation =  rotatorQuat * playerCameraTransform.GetRotation();
	
	m_pInteractWidget->SetWorldRotation(iconRotation);
}

float AInteractionPoint::GetCameraYawTolerance() const
{
	return m_pInteractableInterface->GetCameraYawTolerance();
}

float AInteractionPoint::GetCameraPitchTolerance() const
{
	return m_pInteractableInterface->GetCameraPitchTolerance();
}

void AInteractionPoint::RegisterParent(IInteractableInterface* pInteractableInterface, bool shouldBeEnabled)
{
	m_pInteractableInterface = pInteractableInterface;
	SetIsEnabled(shouldBeEnabled);
}

bool AInteractionPoint::IsShowingNothing() const
{
	return m_CurrentWidgetState == CurrentWidgetState::Hidden;
}

void AInteractionPoint::TryInteract(UInteractionUserComponent* pUser)
{
	if (m_pInteractableInterface->IsFastInteraction())
	{
		Execute_InteractionWidgetInteractFast(this);
		m_pInteractableInterface->OnInteractionStarted(pUser, m_interactorId);
		return;
	}
	Execute_InteractionWidgetInteractSlow(this);
	m_CurrentWidgetState = CurrentWidgetState::Hidden;
	m_pInteractableInterface->OnInteractionStarted(pUser, m_interactorId);

}

bool AInteractionPoint::HasLinkedInteractable() const
{
	return m_pInteractableInterface != nullptr;
}

bool AInteractionPoint::IsFastInteractable() const
{
	return m_pInteractableInterface && m_pInteractableInterface->IsFastInteraction();
}

bool AInteractionPoint::ForceFocus() const
{
	return m_bIsForcedFocused;

}
	
void AInteractionPoint::SetForceFocus(bool set)
{
	m_bIsForcedFocused = set;

}
	
