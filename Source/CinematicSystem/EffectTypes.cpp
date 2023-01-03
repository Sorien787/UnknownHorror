#include "EffectTypes.h"

#include "Common/UnrealUtilities.h"
#include "Components/AudioComponent.h"

void UIEffectType::Initialize()
{
	Initialize_Internal();
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
