#include "TriggerTypes.h"
#include "CinematicEvent.h"
#include "Common/UnrealUtilities.h"
#include "Components/ShapeComponent.h"

void UITriggerType::ChangeTriggeredState(bool triggered)
{
	if (m_IsTriggered == triggered)
		return;
	
	m_IsTriggered = triggered;
	
	if (IsConsideredTriggered())
		OnTriggerCalled.Broadcast(m_TriggerID);
	else
		OnTriggerInvalidated.Broadcast(m_TriggerID);
}

bool UITriggerType::IsConsideredTriggered() const
{
	return m_IsTriggered ^ invertTriggerOutput;
}

uint8 UITriggerType::GetTriggerGroup() const
{
	return m_TriggerGroup;
}

ETriggerPriority UITriggerType::GetTriggerPriority() const
{
	return m_TriggerPriority;
}

void UITriggerType::OnTriggerBecomeValid(ACinematicEvent* parent)
{
	OnTriggerBecomeValid_Internal();
	
	OnTriggerCalled.AddDynamic(parent, &ACinematicEvent::OnTriggerCalled);
	OnTriggerInvalidated.AddDynamic(parent, &ACinematicEvent::OnTriggerUncalled);
	
	if (!IsConsideredTriggered())
		return;
	
	parent->OnTriggerCalled(m_TriggerID);
}

void UITriggerType::OnTriggerBecomeInvalid(ACinematicEvent* parent)
{
	OnTriggerCalled.RemoveDynamic(parent, &ACinematicEvent::OnTriggerCalled);
	OnTriggerInvalidated.RemoveDynamic(parent, &ACinematicEvent::OnTriggerUncalled);
	OnTriggerBecomeInvalid_Internal();
}

void UITriggerType::Initialize(int triggerID)
{
	m_TriggerID = triggerID;
	Initialize_Internal();
}

void UITriggerType::Initialize_Internal()
{
}





void UColliderTriggerType::Initialize_Internal()
{
	m_Shape = UnrealUtilities::GetComponentFromActor<UShapeComponent>(m_ActorReference);
	m_Shape->OnComponentBeginOverlap.AddDynamic(this, &UColliderTriggerType::OnBoxBeginOverlap);
	m_Shape->OnComponentEndOverlap.AddDynamic(this, &UColliderTriggerType::OnBoxEndOverlap);
}

void UColliderTriggerType::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const APawn* pPawn = Cast<APawn>(OtherActor);
	
	if (!pPawn)
		return;
	
	if (!pPawn->IsPlayerControlled())
		return;
	
	ChangeTriggeredState(true);
}
void UColliderTriggerType::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const APawn* pPawn = Cast<APawn>(OtherActor);
	
	if (!pPawn)
		return;
	
	if (!pPawn->IsPlayerControlled())
		return;
	
	ChangeTriggeredState(false);
}

void UDelayTriggerType::Tick(float tickSeconds)
{
	if (m_CurrentTimerTime > m_TimerLength)
		return;
	
	m_CurrentTimerTime += tickSeconds;
	
	if (m_CurrentTimerTime < m_TimerLength)
		return;

	ChangeTriggeredState(true);
}

void UDelayTriggerType::OnTriggerBecomeInvalid_Internal()
{
	ChangeTriggeredState(false);
	m_CurrentTimerTime = 0.0f;
}


void UObjectTraceTriggerType::Tick(float tickSeconds)
{

}

void UObjectOnScreenTriggerType::Tick(float tickSeconds)
{
	ChangeTriggeredState(UnrealUtilities::IsInFrustrum(m_ActorReference->GetActorLocation(), 1.0f, GetWorld()));
}

void UObjectOnScreenTriggerType::OnTriggerBecomeValid_Internal()
{
	ChangeTriggeredState(UnrealUtilities::IsInFrustrum(m_ActorReference->GetActorLocation(), 1.0f, GetWorld()));
}

void UObjectRecentlyRenderedTriggerType::Tick(float tickSeconds)
{
	ChangeTriggeredState(m_ActorReference->WasRecentlyRendered(m_RecentlyRenderedToleranceTime));
}

void UObjectRecentlyRenderedTriggerType::OnTriggerBecomeValid_Internal()
{
	ChangeTriggeredState(m_ActorReference->WasRecentlyRendered(m_RecentlyRenderedToleranceTime));
}
