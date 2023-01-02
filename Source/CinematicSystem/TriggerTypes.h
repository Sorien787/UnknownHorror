#pragma once

#include "CinematicCommon.h"
#include "TriggerTypes.generated.h"

class ACinematicEvent;

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UITriggerType : public UObject
{
	GENERATED_BODY()
	
	bool m_IsTriggered = false;

protected:
	
	size_t m_TriggerID = 0;

	void ChangeTriggeredState(bool triggered);
	
	UPROPERTY(EditAnywhere)
	bool invertTriggerOutput = false;
	
	UPROPERTY()
	FOnTriggerCalled OnTriggerCalled;

	UPROPERTY()
	FOnTriggerInvalidated OnTriggerInvalidated;
	
	UPROPERTY(EditAnywhere)
	ETriggerPriority m_TriggerPriority;
	
	UPROPERTY(EditAnywhere)
	uint8 m_TriggerGroup;

	
public:

	bool IsConsideredTriggered() const;
	
	virtual bool RequiresTick() const {return false; }

	virtual void Tick(float tickSeconds) {}

	void OnTriggerBecomeValid(ACinematicEvent* parent);

	virtual void OnTriggerBecomeValid_Internal() {}

	void OnTriggerBecomeInvalid(ACinematicEvent* parent);

	virtual void OnTriggerBecomeInvalid_Internal() {}
	
	ETriggerPriority GetTriggerPriority() const;

	uint8 GetTriggerGroup() const;

	void Initialize(int triggerID);
	
	virtual void Initialize_Internal();
};

inline uint8 UITriggerType::GetTriggerGroup() const
{
	return m_TriggerGroup;
}

inline ETriggerPriority UITriggerType::GetTriggerPriority() const
{
	return m_TriggerPriority;
}

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UTriggerStageTriggerType : public UITriggerType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	ETriggerStage m_TriggerStage = ETriggerStage::First;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UColliderTriggerType : public UITriggerType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	AActor* m_ActorReference = nullptr;

	UPROPERTY()
	UShapeComponent* m_Shape = nullptr;
	
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void Initialize_Internal() override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UGameStateTriggerType : public UITriggerType
{
	GENERATED_BODY()
	
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UDelayTriggerType : public UITriggerType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float m_TimerLength = 0.0f;

	float m_CurrentTimerTime = 0.0f;
	
	virtual bool RequiresTick() const override { return true; }

	virtual void Tick(float tickSeconds) override;
	
	virtual void OnTriggerBecomeInvalid_Internal() override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UObjectTraceTriggerType : public UITriggerType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	AActor* m_ActorReference = nullptr;

	virtual bool RequiresTick() const override { return true; }

	virtual void Tick(float tickSeconds) override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UObjectOnScreenTriggerType : public UITriggerType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	AActor* m_ActorReference = nullptr;

	virtual bool RequiresTick() const override { return true; }

	virtual void Tick(float tickSeconds) override;

	virtual void OnTriggerBecomeValid_Internal() override;
};


UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UObjectRecentlyRenderedTriggerType : public UITriggerType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	AActor* m_ActorReference = nullptr;

	UPROPERTY(EditAnywhere)
	float m_RecentlyRenderedToleranceTime = 0.1f;

	virtual bool RequiresTick() const override { return true; }

	virtual void Tick(float tickSeconds) override;

	virtual void OnTriggerBecomeValid_Internal() override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class USchismStateTriggerType : public UITriggerType
{
	GENERATED_BODY()
	
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UPlayerInteractingTriggerType : public UITriggerType
{
	GENERATED_BODY()
	
};
// PlayerLook,
// PlayerInteract,
// ObjectState