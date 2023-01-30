// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionPoint.h"

#include "InteractionComponentInterface.h"
#include "../Common/UnrealUtilities.h"
#include "Kismet/KismetMathLibrary.h"


static TAutoConsoleVariable<int32> InteractionPointDebug(
	TEXT("InteractionPointDebug"),
	0,
	TEXT("InteractionPointDebug.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);


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

	m_InitialTriggerBoxComponent= CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Trigger Start Box"));
	check(m_InitialTriggerBoxComponent != nullptr);
	m_InitialTriggerBoxComponent->SetupAttachment(GetRootComponent());

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
	m_CurrentWidgetState = CurrentWidgetState::Revealed;
	Execute_InteractionWidgetReveal(this);
	
	if (GEngine && InteractionPointDebug.GetValueOnGameThread())
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Trying Revealing Widget"));
}

void AInteractionPoint::TryHideWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Hidden)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Hidden;
	Execute_InteractionWidgetHide(this);

		
	if (GEngine && InteractionPointDebug.GetValueOnGameThread())
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Trying Hiding Widget"));
}

void AInteractionPoint::TryFocusWidget()
{
	if (m_CurrentWidgetState == CurrentWidgetState::Interactable || !m_bIsCurrentlyActive)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Interactable;
	Execute_InteractionWidgetFocus(this);
	
	m_InitialTriggerBoxComponent->SetWorldScale3D(FVector(1.1f));	
	
	if (GEngine && InteractionPointDebug.GetValueOnGameThread())
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Trying Focusing Widget"));
}

void AInteractionPoint::TryUnfocusWidget()
{
	if (m_CurrentWidgetState != CurrentWidgetState::Interactable || !m_bIsCurrentlyActive)
		return;
	m_CurrentWidgetState = CurrentWidgetState::Revealed;
	Execute_InteractionWidgetUnfocus(this);

	m_InitialTriggerBoxComponent->SetWorldScale3D(FVector(1.0f));	
	
	if (GEngine && InteractionPointDebug.GetValueOnGameThread())
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Trying Unfocusing Widget"));
}

bool AInteractionPoint::GetActorTriggerConditionMet(FTransform actorTransform, FVector actorVelocity) const
{
	const FTransform& interactionPointActorTransform = GetActorTransform();
	const FVector interactionPointForwardVector = interactionPointActorTransform.GetUnitAxis(EAxis::X);
	const FVector interactionPointPosition = interactionPointActorTransform.GetLocation();

	FVector interactionPoint_to_otherActor = actorTransform.GetLocation() - interactionPointPosition;
	if (!interactionPoint_to_otherActor.Normalize())
		return true;

	const float angleBetweenForwardAndActor = FMath::Acos(FVector::DotProduct(interactionPoint_to_otherActor, interactionPointForwardVector));

	return FMath::RadiansToDegrees(angleBetweenForwardAndActor) < m_TriggerAngle;
}

void AInteractionPoint::Tick(float DeltaSeconds)
{
	const FTransform playerCameraTransform = UnrealUtilities::GetPlayerCameraTransform(GetWorld());
	const FQuat rotatorQuat = FQuat(playerCameraTransform.GetUnitAxis(EAxis::Z), PI);
	const FQuat iconRotation =  rotatorQuat * playerCameraTransform.GetRotation();
	
	m_pInteractWidget->SetWorldRotation(iconRotation);

	if (!InteractionPointDebug.GetValueOnGameThread())
		return;

	FColor enterBoxColor = FColor::White;
	if (m_CurrentWidgetState != CurrentWidgetState::Interactable)
	{
		enterBoxColor = FColor::Green;
	}
	DrawDebugBox(GetWorld(), m_InitialTriggerBoxComponent->GetComponentLocation(), m_InitialTriggerBoxComponent->GetScaledBoxExtent(), enterBoxColor);
}

void AInteractionPoint::TryInteract(IInteractionComponentInterface* pUser)
{
	Execute_InteractionWidgetInteractSlow(this);
	m_CurrentWidgetState = CurrentWidgetState::Hidden;
	TScriptInterface<IInteractionComponentInterface> result;
	result.SetInterface(pUser);
	result.SetObject(pUser->GetComponentObject());
	m_pInteractableInterface->OnInteractionStarted(result, GetActorLocation(), FQuat(GetActorRotation()),  m_interactorId);
    
}
	
