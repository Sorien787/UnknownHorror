#pragma once

#include "CinematicCommon.h"
#include "TriggerTypes.generated.h"

class ACinematicEvent;

UCLASS()
class UITriggerType : public UObject
{
	GENERATED_BODY()
	
	size_t m_TriggerID = 0;
	
public:
	virtual bool RequiresTickCheck() const {return false; }

	virtual void Tick() {}

	void Initialize(ACinematicEvent* parent, int triggerID);
	
	virtual void Initialize_Internal(ACinematicEvent* parent);
};

UCLASS()
class UTriggerStageTriggerType : public UITriggerType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	ETriggerStage m_TriggerStage = ETriggerStage::First;
};

UCLASS()
class UColliderTriggerType : public UITriggerType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	UShapeComponent* m_Collider = nullptr;

	UPROPERTY(EditAnywhere)
	bool m_ActiveWhenInside = true;
	
	virtual void Initialize_Internal(ACinematicEvent* parent) override;
};