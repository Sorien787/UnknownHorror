#include "EffectTypes.h"

#include "CinematicEvent.h"
#include "Common/UnrealUtilities.h"
#include "Components/AudioComponent.h"
#include "Haze/HazeComponent.h"
#include "Haze/HazeEffectComponent.h"

void UIEffectType::Initialize(ACinematicEvent* pParentEvent)
{
	m_pParentEvent = pParentEvent;
	Initialize_Internal();
}

void UIEffectItemType::OnEffectTriggered()
{
	m_pParentEvent->RegisterOngoingEvent(this);
	UItemControlComponent* pComponent = UnrealUtilities::GetComponentFromActor<UItemControlComponent>(m_pControlledActor);
	pComponent->RequestItemControl(this);
}

int UIEffectItemType::GetPriority() const
{
	return m_pParentEvent->GetDefaultItemPriority();
}

AActor* UIEffectItemType::GetControlledItem() const
{
	return m_pControlledActor;
}

void UIEffectItemType::CancelItemControl()
{
	UItemControlComponent* pComponent = UnrealUtilities::GetComponentFromActor<UItemControlComponent>(m_pControlledActor);
	pComponent->ReleaseItemControl(this);
}

void URelinquishItemControlEffectType::OnEffectTriggered()
{
	m_pParentEvent->RelinquishControlForActor(m_pControlledActor);
}

void ULightModifierEffectType::OnEffectTriggered()
{
	UnrealUtilities::GetComponentFromActor<ULightModifierComponent>(pRelevantActor)->SetFlickerStatusOverride(LightFlickerOverrideState);
}

void ULightModifierBreakType::OnEffectTriggered()
{
	UnrealUtilities::GetComponentFromActor<ULightModifierComponent>(pRelevantActor)->Break();
}

void USoundEffectType::OnEffectTriggered()
{
	UnrealUtilities::GetComponentFromActor<UAudioComponent>(pRelevantActor)->Play();
}

void UParticleEffectType::OnEffectTriggered()
{
	// UnrealUtilities::GetComponentFromActor<UNiagaraComponent>(pRelevantActor)->Activate();
}

void UHazeStrengthEffectType::OnEffectTriggered()
{
	UnrealUtilities::GetComponentFromActor<UHazeComponent>(pRelevantActor)->m_HazeStrengthMultiplier = HazeStrength;
}
