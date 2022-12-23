#include "TriggerTypes.h"
#include "CinematicEvent.h"

void UITriggerType::Initialize(ACinematicEvent* parent, int triggerID)
{
	m_TriggerID = triggerID;
	Initialize_Internal(parent);
}

void UITriggerType::Initialize_Internal(ACinematicEvent* parent)
{
}





void UColliderTriggerType::Initialize_Internal(ACinematicEvent* parent)
{
	
}
