#pragma once

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
	Zero,
	First,
	Second,
	Third,
	Fourth,
	Fifth,
	Sixth,
	Exit
};