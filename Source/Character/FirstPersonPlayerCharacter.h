// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
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

	void OnEnterLookState();

	void OnExitLookState();

	FRotator ApplyPitchYawOffsetToBase(FRotator base) const;
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


	// FPS camera
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* m_CharacterCamera;
	
	UPROPERTY(EditAnywhere)
		USpringArmComponent* m_pCameraBoomArm;
	
	UPROPERTY(VisibleAnywhere)
		UInteractionUserComponent* m_InteractionUserComponent;
	
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_EnterBoxComponent;
	
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_ExitBoxComponent;

	UPROPERTY(EditAnywhere)
		float m_DefaultCrouchSpeed = 100.0f;
	
	UPROPERTY(EditAnywhere)
		float m_DefaultSpeed = 150.0f;

	UPROPERTY(EditAnywhere)
		float m_SprintSpeed = 400.0f;

	UPROPERTY(EditAnywhere)
		float m_HeadSnapBackSpeed = 100.0f;
	
	UPROPERTY(EditAnywhere)
		float m_ShoulderStartAngle = 100.0f;

	UPROPERTY(EditAnywhere)
		float m_ShoulderEndAngle = 100.0f;
	UPROPERTY(EditAnywhere)
	
	float m_ShoulderLowAngle = -10.0f;
	
	UPROPERTY(EditAnywhere)
		float m_MaxHeadYaw = 120.0f;
	
	UPROPERTY(EditAnywhere)
		float m_MaxHeadPitch = 80.0f;

	UPROPERTY(EditAnywhere)
		float m_LookSensitivityVal = 0.1f;
	
	bool m_bWantsToSprint{false};

	bool m_bIsSprinting{false};

	bool m_bWantsToCrouch{false};

	bool m_bForcedToCrouch{false};


	bool m_bForcedIntoStaticCamera{false};

	bool m_bMouseInputToActorRotation{false};

	float m_currentYawOffset = 0.0f;
	float m_currentPitchOffset = 0.0f;

	float m_currentMaxYawOffset = 0.0f;
	float m_currentMaxPitchOffset = 0.0f;
	
	bool m_bIsLooking{false};
	bool m_bWantsToLook{false};
	
	bool m_bIsHeadAlignedWithBody{true};

	bool m_bIsInputLocked{false};
	FRotator m_cachedLockRotation;
};

