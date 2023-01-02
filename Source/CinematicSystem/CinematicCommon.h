#pragma once

#include "CoreMinimal.h"
#include "CinematicCommon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerCalled, int, triggerID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerInvalidated, int, triggerID);

class ITriggerListener
{
	void OnTriggerCalled(int triggerID);
};

UENUM(BlueprintType)
enum class ETriggerPriority : uint8
{
	Low,
	Medium,
	High
};

UENUM(BlueprintType)
enum class EEffectReset : uint8
{
	NoReset,
	ResetIfNoSubsequentMatch
};

UENUM(BlueprintType)
enum class ETriggerTypes : uint8
{
	TriggerStage,
	GameState,
	CollisionVolume,
	PlayerPos,
	PlayerLook,
	PlayerInteract,
	Delay,
	ObjectVisibility,
	SchismState,
	ObjectState
};

UENUM(BlueprintType)
enum class EEffectTypes : uint8
{
	SetLightManualOverride,
	PlaySoundSingle,
	PlaySoundOngoing,
	SpawnSchismChasing,
	SpawnSchismLurking,
	SchismPoundingDoor,
	SchismEyesBehindPipe,
	SchismLurkingBehindDoor,
	// set door state, topple shelves, burst pipe, etc.
	Blueprint
};

UENUM(BlueprintType)
enum class ETriggerStage : uint8
{
	Any,
	Zero,
	First,
	Second,
	Third,
	Fourth,
	Fifth,
	Sixth,
	Exit
};