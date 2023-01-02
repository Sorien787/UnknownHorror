// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Haze/HazeEffectComponent.h"
#include "Interaction/InteractionUserComponent.h"
#include "FirstPersonPlayerCharacter.generated.h"

UCLASS()
class DEEPSEAHORROR_API AFirstPersonPlayerCharacter : public ACharacter
{
	GENERATED_BODY()
private:
	void ClampViewOffsetToShoulder(float DeltaTime);

	void ClampViewOffsetToMaximum(float maximumPitch, float maximumYaw);

	void ClampViewOffsetToZero(float DeltaTime);

	FQuat m_currentHeadQuat;

	FVector4 m_currentHeadVelocity;

public:
	// Sets default values for this character's properties
	AFirstPersonPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PawnClientRestart() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// INPUT ACTIONS //
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* MovementForwardAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* MovementRightAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* LookUpAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* LookSideAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* JumpAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* CrouchAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* ProneAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* InteractionAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* LookAroundAction;

	// INPUT MAPPINGS //
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mappings")
	UInputMappingContext* BaseMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mappings")
	int32 BaseMappingPriority = 0;
	
	bool CanEnterSprint() const;

	void SprintStateUpdate(float DeltaTime);

	void CrouchStateUpdate(float DeltaTime);

	void LookStateUpdate(float DeltaTime);

	void InteractionStateUpdate(float DeltaTime);

	void HazeDispersalUpdate(float DeltaTime);
	
	void CameraEffectsUpdate(float DeltaTime);

	void AudioEffectsUpdate(float DeltaTime);

	// INPUT FUNCTIONS //
	void MoveForward(const FInputActionValue& value);
	void MoveRight(const FInputActionValue& value);
	void LookUp(const FInputActionValue& value);
	void LookRight(const FInputActionValue& value);
	void AddCharacterPitchInput(float pitch);
	void AddCharacterYawInput(float yaw);

	void StartJump();
	void StopJump();
	
	void StartSprint();
	void StopSprint();
	
	void StartLookAround();
	void StopLookAround();

	void TriggerInteractions();
	
	void ToggleCrouch();
	
	void ToggleProne();


	UFUNCTION(BlueprintCallable)
		void BeginCrouch();

	UFUNCTION(BlueprintCallable)
		void EndCrouch();

	UFUNCTION()
		void LockCamera(float maxYaw, float maxPitch);
	
	UFUNCTION()
		void UnlockCamera();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* m_CharacterCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		USpringArmComponent* m_pCameraBoomArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		UInteractionUserComponent* m_InteractionUserComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		UHazeEffectComponent* m_pHazeEffectComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		USphereComponent* m_EnterInteractionSphereCollider;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		USphereComponent* m_ExitInteractionSphereCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		UAudioComponent* m_HeartbeatAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		UAudioComponent* m_RingingAudioComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		UAudioComponent* m_DroneAudioComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Animation Setup")
		UMaterialParameterCollection* m_VisualsMaterialParameterCollection;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Audio setup")
		USoundClass* m_InWorldSoundClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Audio setup")
		FRuntimeFloatCurve m_AudioLowPassByHazeStrength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Animation Setup")
		FRuntimeFloatCurve m_HazeEffectPulseProfile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Animation Setup")
		FRuntimeFloatCurve m_HazeEffectPulseTimeByHazeStrength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Animation Setup")
		FRuntimeFloatCurve m_HazeEffectBloomByHazeStrength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Animation Setup")
		FRuntimeFloatCurve m_HazeEffectChromaticAbberation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Audio Setup")
		FRuntimeFloatCurve m_HazeEffectPulseVolumeByHazeStrength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Audio Setup")
		FRuntimeFloatCurve m_HazeEffectPulsePitchByHazeStrength;

	UnrealUtilities::RisingEdgeTrigger<float> m_HeartBeatSFXTrigger;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Audio Setup")
		FRuntimeFloatCurve m_HazeEffectRingingVolumeByHazeStrength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Audio Setup")
		FRuntimeFloatCurve m_HazeEffectDroneVolumeByHazeStrength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Haze Setup")
		FRuntimeFloatCurve m_HazeDispersalByCurrentHaze;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Haze Setup")
		float m_MaximumGainedHaze;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Haze Setup")
		float m_MaximumHazeApplication;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Setup")
		float m_CameraLookNormalizedSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Setup")
		float m_CameraShoulderReturnSpeeds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Movement Setup")
		float m_DefaultCrouchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Movement Setup")
		float m_DefaultSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Movement Setup")
		float m_SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Setup")
		float m_HeadSnapBackSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Setup")
		float m_ShoulderStartAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Setup")
		float m_ShoulderEndAngle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Setup")
	float m_ShoulderLowAngle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Setup")
		float m_MaxHeadYaw;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Setup")
		float m_MaxHeadPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Setup")
		float m_LookSensitivityVal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Camera Setup")
		FName m_HeadBoneName;
	
	bool m_bWantsToSprint;

	bool m_bIsSprinting;

	bool m_bWantsToCrouch;

	bool m_bForcedToCrouch;


	bool m_bForcedIntoStaticCamera;

	bool m_bMouseInputToActorRotation;

	float m_currentYawOffset;
	float m_currentPitchOffset;

	float m_fCurrentHaze;
	float m_fCurrentHeartbeatTime;

	float m_currentMaxYawOffset;
	float m_currentMaxPitchOffset;
	
	bool m_bIsLooking;
	bool m_bWantsToLook;

	bool m_bIsInputLocked;
};

