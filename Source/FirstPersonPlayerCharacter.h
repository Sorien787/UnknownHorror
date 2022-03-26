// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "InteractionUserComponent.h"
#include "FirstPersonPlayerCharacter.generated.h"

UCLASS()
class DEEPSEAHORROR_API AFirstPersonPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFirstPersonPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handles input for moving forward and backward.
	UFUNCTION()
		void MoveForward(float Value);

	// Handles input for moving right and left.
	UFUNCTION()
		void MoveRight(float Value);

	// Sets jump flag when key is pressed.
	UFUNCTION()
		void StartJump();

	UFUNCTION()
		void ToggleCrouch();

	// Clears jump flag when key is released.
	UFUNCTION()
		void StopJump();

	bool CanEnterSprint() const;

	void SprintStateUpdate();
	
	UFUNCTION()
		void SprintStart();
	
	UFUNCTION()
		void SprintEnd();

	// Triggers Interaction stuff
	UFUNCTION()
		void StartInteractions();

	// FPS camera
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* m_CharacterCamera;

	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* m_pCharacterMesh;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* m_pCameraBoomArm;
	
	UPROPERTY(EditAnywhere)
		UInteractionUserComponent* m_InteractionComponent;
	
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

	bool m_bWantsToSprint{false};

	bool m_bIsSprinting{false};
};

