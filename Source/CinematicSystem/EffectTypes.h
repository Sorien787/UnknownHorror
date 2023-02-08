#pragma once
#include "Gameplay/LightModifierComponent.h"
#include "ItemControl/ItemControlComponent.h"
#include "EffectTypes.generated.h"


class ACinematicEvent;

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class UIEffectType : public UObject
{
	GENERATED_BODY()
protected:
	ACinematicEvent* m_pParentEvent;
public:
	void Initialize(ACinematicEvent* pParentEvent);
	
	virtual void Initialize_Internal() {}

	virtual void OnEffectTriggered() {}

	virtual void OnCancelEffect() {}

	virtual void Reset() {}
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class UIEffectItemType : public UIEffectType, public IItemControlRequester
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere)
	AActor* m_pControlledActor = nullptr;

	virtual void OnEffectTriggered() override;

	virtual int GetPriority() const override;

	virtual AActor* GetControlledItem() const;

	void CancelItemControl();
};
	
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class URelinquishItemControlEffectType : public UIEffectType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	AActor* m_pControlledActor;
	
public:
	
	virtual void OnEffectTriggered() override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class ULightModifierEffectType : public UIEffectType
{
	GENERATED_BODY()


	virtual void OnEffectTriggered() override;

	UPROPERTY(EditAnywhere)
	AActor* pRelevantActor;

	UPROPERTY(EditAnywhere)
	FLightFlickerStateStruct LightFlickerOverrideState;
	
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class ULightModifierBreakType : public UIEffectType
{
	GENERATED_BODY()
	
	virtual void OnEffectTriggered() override;

	UPROPERTY(EditAnywhere)
	AActor* pRelevantActor;
	
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class USoundEffectType : public UIEffectType
{
	GENERATED_BODY()
	// fade in/fade out options
	//
	UPROPERTY(EditAnywhere)
	AActor* pRelevantActor;
	
	virtual void OnEffectTriggered() override;

	UPROPERTY()
	float FadeTime = 0.0f;

	UPROPERTY()
	float VolumeModifier = 0.0f;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UHazeStrengthEffectType : public UIEffectType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	AActor* pRelevantActor;

	virtual void OnEffectTriggered() override;

	UPROPERTY()
	float HazeStrength = 0.0f;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UParticleEffectType : public UIEffectType
{
	GENERATED_BODY()
	// fade in/fade out options
	//
	UPROPERTY(EditAnywhere)
	AActor* pRelevantActor;
	
	virtual void OnEffectTriggered() override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UObjectFallEffectType : public UIEffectType
{
	GENERATED_BODY()
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UPipeBurstEffectType : public UIEffectType
{
	GENERATED_BODY()
};
// emergency lighting?
// maybe could do a power network (or something)
// for now just keep it simple