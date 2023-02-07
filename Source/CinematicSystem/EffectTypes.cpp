#include "EffectTypes.h"

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
	m_pParentEvent->RegisterOngoingEvent();
	UItemControlComponent* pComponent = UnrealUtilities::GetComponentFromActor<UItemControlComponent>(pControlledActor);
	pComponent->RequestItemControl(this);
}

int UIEffectItemType::GetPriority() const
{
	return m_pParentEvent->GetDefaultItemPriority();
}

AActor* UIEffectItemType::GetControlledItem() const
{
	return pControlledActor;
}

void UIEffectItemType::CancelItemControl()
{
	UItemControlComponent* pComponent = UnrealUtilities::GetComponentFromActor<UItemControlComponent>(pControlledActor);
	pComponent->ReleaseItemControl(this);
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
