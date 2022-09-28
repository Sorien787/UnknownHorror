// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FirstPersonPlayerCharacter.h"

#include "DrawDebugHelpers.h"
#include "Interaction/InteractionUserComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

void AFirstPersonPlayerCharacter::ClampViewOffsetToShoulder(float DeltaTime)
{
	if (FMath::Abs(m_currentPitchOffset) - m_currentMaxPitchOffset > 0)
	{
		m_currentPitchOffset = FMath::Sign(m_currentPitchOffset) * (FMath::Abs(m_currentPitchOffset) - FMath::Min(m_HeadSnapBackSpeed * DeltaTime, FMath::Abs(m_currentPitchOffset) - m_currentMaxPitchOffset));
	}
		
	if (FMath::Abs(m_currentYawOffset) - m_currentMaxYawOffset > 0)
	{
		m_currentYawOffset = FMath::Sign(m_currentYawOffset) * (FMath::Abs(m_currentYawOffset) - FMath::Min(m_HeadSnapBackSpeed * DeltaTime, FMath::Abs(m_currentYawOffset) - m_currentMaxYawOffset));
	}

	bool isBelow = m_currentPitchOffset < m_ShoulderLowAngle;
	bool isBeforeShoulder = FMath::Abs(m_currentYawOffset) < m_ShoulderStartAngle;
	bool isAfterShoulder = FMath::Abs(m_currentYawOffset) > m_ShoulderEndAngle;
	if (!isBeforeShoulder && isAfterShoulder && isBelow)
	{
		m_currentPitchOffset = FMath::Clamp(m_currentPitchOffset, m_ShoulderLowAngle, 90.0f);
	}
	else if (!isBeforeShoulder && !isAfterShoulder && isBelow)
	{
		const float percentageOfShoulderYaw = FMath::Min(1.0f,(FMath::Abs(m_currentYawOffset) - m_ShoulderStartAngle) / (m_ShoulderEndAngle - m_ShoulderStartAngle));
		float pitchRange = m_ShoulderLowAngle + m_currentMaxPitchOffset;
		float allowedPitch = -m_currentMaxPitchOffset + percentageOfShoulderYaw * pitchRange;
		if (m_currentPitchOffset < allowedPitch)
			m_currentPitchOffset = allowedPitch;
	}
}

void AFirstPersonPlayerCharacter::ClampViewOffsetToZero(float DeltaTime)
{
	m_currentYawOffset = 0.0f;
	m_currentPitchOffset = 0.0f;
}

// Sets default values
AFirstPersonPlayerCharacter::AFirstPersonPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pCameraBoomArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Cam Boom Arm"));
	m_pCameraBoomArm->SetupAttachment(GetMesh(), "cameraSocket");
	
	m_CharacterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(m_CharacterCamera != nullptr);
	m_CharacterCamera->SetupAttachment(m_pCameraBoomArm, USpringArmComponent::SocketName);
	m_CharacterCamera->bUsePawnControlRotation = false;
	
	m_InteractionUserComponent = CreateDefaultSubobject<UInteractionUserComponent>(TEXT("Interaction User"));
	check(m_InteractionUserComponent != nullptr);
	
	m_EnterBoxComponent= CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Enter Box"));
	check(m_EnterBoxComponent != nullptr);
	m_EnterBoxComponent->SetupAttachment(GetRootComponent());

	m_ExitBoxComponent= CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Exit Box"));
	check(m_ExitBoxComponent != nullptr);
	m_ExitBoxComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AFirstPersonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	const APlayerController* PC = Cast<const APlayerController>(Controller);

	PC->PlayerCameraManager->ViewPitchMin = -m_MaxHeadPitch;
	PC->PlayerCameraManager->ViewPitchMax = m_MaxHeadPitch;
	
	m_currentMaxPitchOffset = m_MaxHeadPitch;
	m_currentMaxYawOffset = m_MaxHeadYaw;
	
	GetCharacterMovement()->MaxWalkSpeed = m_DefaultSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = m_DefaultCrouchSpeed;

	m_InteractionUserComponent->AddInteractionEnterBox(m_EnterBoxComponent);
	m_InteractionUserComponent->AddInteractionExitBox(m_ExitBoxComponent);
}

// Called every frame
void AFirstPersonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SprintStateUpdate(DeltaTime);
	LookStateUpdate(DeltaTime);
	CrouchStateUpdate(DeltaTime);
	InteractionStateUpdate(DeltaTime);
}

// Called to bind functionality to input
void AFirstPersonPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* eic = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Super::SetupPlayerInputComponent(PlayerInputComponent);	

	eic->BindAction( JumpAction, ETriggerEvent::Started, this, &AFirstPersonPlayerCharacter::StartJump);
	eic->BindAction( JumpAction, ETriggerEvent::Completed, this, &AFirstPersonPlayerCharacter::StopJump);

	eic->BindAction( SprintAction, ETriggerEvent::Started, this, &AFirstPersonPlayerCharacter::StartSprint);
	eic->BindAction( SprintAction, ETriggerEvent::Completed, this, &AFirstPersonPlayerCharacter::StopSprint);

	eic->BindAction( LookAroundAction, ETriggerEvent::Started, this, &AFirstPersonPlayerCharacter::StartLookAround);
	eic->BindAction( LookAroundAction, ETriggerEvent::Completed, this, &AFirstPersonPlayerCharacter::StopLookAround);

	eic->BindAction( InteractionAction, ETriggerEvent::Triggered, this, &AFirstPersonPlayerCharacter::TriggerInteractions);
	
	eic->BindAction( CrouchAction, ETriggerEvent::Triggered, this, &AFirstPersonPlayerCharacter::ToggleCrouch);
	
	eic->BindAction( ProneAction, ETriggerEvent::Triggered, this, &AFirstPersonPlayerCharacter::ToggleProne);

	eic->BindAction( LookUpAction, ETriggerEvent::Triggered, this, &AFirstPersonPlayerCharacter::LookUp);
	eic->BindAction( LookSideAction, ETriggerEvent::Triggered, this, &AFirstPersonPlayerCharacter::LookRight);

	eic->BindAction( MovementForwardAction, ETriggerEvent::Triggered, this, &AFirstPersonPlayerCharacter::MoveForward);
	eic->BindAction( MovementRightAction, ETriggerEvent::Triggered, this, &AFirstPersonPlayerCharacter::MoveRight);
}

void AFirstPersonPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
		return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem)
		return;

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(BaseMappingContext, BaseMappingPriority);
}


void AFirstPersonPlayerCharacter::AddCharacterPitchInput(float pitch)
{
	pitch *= m_LookSensitivityVal;
	if (m_bIsLooking)
	{
		m_currentPitchOffset += -pitch;
		m_currentPitchOffset = FMath::Clamp(m_currentPitchOffset, -m_currentMaxPitchOffset, m_currentMaxPitchOffset);
	}
	else
	{
		AddControllerPitchInput(pitch);
	}
}

void AFirstPersonPlayerCharacter::AddCharacterYawInput(float yaw)
{
	yaw *= m_LookSensitivityVal;
	if (m_bIsLooking)
	{
		m_currentYawOffset += yaw;
		m_currentYawOffset = FMath::Clamp(m_currentYawOffset, -m_currentMaxYawOffset, m_currentMaxYawOffset);
	}
	else
	{
		AddControllerYawInput(yaw);
	}
}

void AFirstPersonPlayerCharacter::MoveForward(const FInputActionValue& value)
{
	if (m_bIsInputLocked || value.GetMagnitude() == 0.0f)
		return;
	
	const FVector Forward = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Forward, value[0]);
}

void AFirstPersonPlayerCharacter::MoveRight(const FInputActionValue& value)
{
	if (m_bIsInputLocked || value.GetMagnitude() == 0.0f)
		return;

	const FVector Right = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Right, value[0]);
}

void AFirstPersonPlayerCharacter::LookRight(const FInputActionValue& value)
{
	if (value.GetMagnitude() == 0.0f)
		return;
	AddCharacterYawInput(value[0]);
}

void AFirstPersonPlayerCharacter::LookUp(const FInputActionValue& value)
{
	if (value.GetMagnitude() == 0.0f)
		return;
	AddCharacterPitchInput(value[0]);
}	

void AFirstPersonPlayerCharacter::BeginCrouch()
{
	m_bWantsToCrouch = true;
}

void AFirstPersonPlayerCharacter::EndCrouch()
{
	m_bWantsToCrouch = false;
}

void AFirstPersonPlayerCharacter::ToggleCrouch()
{
	if (m_bIsInputLocked)
		return;
	
	m_bWantsToCrouch = !m_bWantsToCrouch;
}

void AFirstPersonPlayerCharacter::ToggleProne()
{
	if (m_bIsInputLocked)
		return;
	
	m_bWantsToCrouch = !m_bWantsToCrouch;
}


void AFirstPersonPlayerCharacter::StartSprint()
{
	m_bWantsToSprint = true;
}

void AFirstPersonPlayerCharacter::StopSprint()
{
	m_bWantsToSprint = false;
}

void AFirstPersonPlayerCharacter::StartJump()
{
	if (m_bIsInputLocked)
		return;
	
	bPressedJump = true;
}

void AFirstPersonPlayerCharacter::StopJump()
{
	bPressedJump = false;
}

void AFirstPersonPlayerCharacter::StartLookAround()
{
	m_bWantsToLook = true;
}

void AFirstPersonPlayerCharacter::StopLookAround()
{
	m_bWantsToLook = false;
}

void AFirstPersonPlayerCharacter::TriggerInteractions()
{
	if (!m_InteractionUserComponent)
		return;
	m_InteractionUserComponent->OnInteractButtonPressed();
}

void AFirstPersonPlayerCharacter::InteractionStateUpdate(float DeltaTime)
{
	if (!m_InteractionUserComponent)
		return;
	
	const bool bShouldLockCamera = m_InteractionUserComponent->IsCurrentlyInInteractionAnimation();
	if (bShouldLockCamera && !m_bIsInputLocked)
	{
		LockCamera(m_InteractionUserComponent->GetAnimCameraYaw(), m_InteractionUserComponent->GetAnimCameraPitch());		
	}
	if (!bShouldLockCamera && m_bIsInputLocked)
	{
		UnlockCamera();
	}
}

void AFirstPersonPlayerCharacter::LookStateUpdate(float DeltaTime)
{
	const bool bCanEnterLook = true;
	const bool tryEnterLookState = m_bIsInputLocked || m_bWantsToLook;

	// use the controller for !m_bIsLooking

	// use the head for m_bIsLooking, + the additional angles

	if (tryEnterLookState && !m_bIsLooking && bCanEnterLook)
	{
		m_bIsLooking = true;

		// we'll be looking towards the control rotation
		// we want to look relative to our head bone
		// so figure out head bone to control rotation
		// and set our current pitch offset and yaw offsets to match
		const FQuat controlQuat = FQuat(GetControlRotation());
	
		const FTransform cameraBoneTransform = GetMesh()->GetBoneTransform(GetMesh()->GetBoneIndex(m_HeadBoneName));
		const FQuat cameraBoneQuat = cameraBoneTransform.GetRotation();

		const FQuat controlQuat_to_cameraBoneQuat = controlQuat.Inverse() * cameraBoneQuat;

		const float pitch = controlQuat_to_cameraBoneQuat.Euler().X;
		const float yaw = controlQuat_to_cameraBoneQuat.Euler().Y;

		m_currentPitchOffset = pitch;
		m_currentYawOffset = yaw;
	}
	
	if (!tryEnterLookState && m_bIsLooking)
	{
		// we'll be looking along head bone + offsets
		// but we want to be looking along our control rotation
		// so set our control rotation to match our look state as best as possible
		
		const FTransform cameraBoneTransform = GetMesh()->GetBoneTransform(GetMesh()->GetBoneIndex(m_HeadBoneName));
		const FQuat cameraBoneQuat = cameraBoneTransform.GetRotation();
	
		const FQuat extraQuat = FQuat::MakeFromEuler(FVector(m_currentPitchOffset, m_currentYawOffset, 0.0f));
		const FQuat totalQuat = cameraBoneQuat * extraQuat;
	
		const float pitch = totalQuat.Euler().X;
		const float yaw = totalQuat.Euler().Y;

		const FQuat controlQuat = FQuat::MakeFromEuler(FVector(pitch, yaw, 0.0f));
	
		Controller->SetControlRotation(FRotator(controlQuat));
	}


	FQuat targetHeadQuat;

	if (m_bIsLooking)
	{
		const FTransform headBoneTransform = GetMesh()->GetBoneTransform(GetMesh()->GetBoneIndex(m_HeadBoneName));
		const FQuat headBoneRotation = headBoneTransform.GetRotation();
		
		ClampViewOffsetToShoulder(DeltaTime);
		
		const FQuat extraQuat = FQuat::MakeFromEuler(FVector(m_currentPitchOffset, m_currentYawOffset, 0.0f));
		const FQuat totalQuat = headBoneRotation * extraQuat;
	
		const float pitch = totalQuat.Euler().X;
		const float yaw = totalQuat.Euler().Y;

		targetHeadQuat = FQuat::MakeFromEuler(FVector(pitch, yaw, 0.0f));		
	}
	else
	{
		const FQuat rotation = FQuat(GetControlRotation());
		
		ClampViewOffsetToZero(DeltaTime);
	
		const FQuat extraQuat = FQuat::MakeFromEuler(FVector(m_currentPitchOffset, m_currentYawOffset, 0.0f));
		const FQuat totalQuat = rotation * extraQuat;
	
		const float pitch = totalQuat.Euler().X;
		const float yaw = totalQuat.Euler().Y;

		targetHeadQuat = FQuat::MakeFromEuler(FVector(pitch, yaw, 0.0f));
	}

	const FQuat currentHeadQuat = FQuat(m_CharacterCamera->GetComponentRotation());
	m_CharacterCamera->SetWorldRotation(targetHeadQuat);
}

void AFirstPersonPlayerCharacter::CrouchStateUpdate(float DeltaTime)
{
	UCharacterMovementComponent* pCharacterMovement = GetCharacterMovement();
	m_bForcedToCrouch = false;

	// if we're not crouching, check if we need to crouch to move underneath something that we're near
	if (!pCharacterMovement->IsCrouching())
	{
		// init params
		const UCapsuleComponent* MyCapsuleComponent = GetCapsuleComponent();
		const float SweepInflation =15.0f;
		const ACharacter* DefaultCharacter = GetClass()->GetDefaultObject<ACharacter>();
		const UWorld* MyWorld = GetWorld();
		const float UnscaledHalfHeight = MyCapsuleComponent->GetUnscaledCapsuleHalfHeight();
		const float ComponentScale = MyCapsuleComponent->GetShapeScale();
		const float ScaledHalfHeightAdjust = UnscaledHalfHeight * ComponentScale;
	
		float Radius, HalfHeight;
		MyCapsuleComponent->GetScaledCapsuleSize(Radius, HalfHeight);
		const FVector PawnLocation = GetActorLocation() + FVector(0, 0, SweepInflation);// + HalfHeight;// + SweepInflation;
		
		FVector CapsuleExtent(Radius, Radius, ScaledHalfHeightAdjust);

		// generate collision params
		FCollisionShape CapsuleCollision = FCollisionShape::MakeCapsule(CapsuleExtent);
		const ECollisionChannel CollisionChannel = GetMesh()->GetCollisionObjectType();
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, this);
		CapsuleParams.bDebugQuery = true;
		FCollisionResponseParams ResponseParam;
		GetMesh()->InitSweepCollisionParams(CapsuleParams, ResponseParam);
		DrawDebugCapsule(MyWorld, PawnLocation, CapsuleExtent.Z, CapsuleExtent.X, FQuat::Identity, FColor::Emerald);
		// make the overlap test
		m_bForcedToCrouch = MyWorld->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, ECC_WorldStatic, CapsuleCollision, CapsuleParams, ResponseParam);
	}
	pCharacterMovement->bWantsToCrouch = m_bWantsToCrouch || m_bForcedToCrouch;
}


void AFirstPersonPlayerCharacter::SprintStateUpdate(float DeltaTime)
{
	if (m_bWantsToSprint && !m_bForcedToCrouch)
	{
		GetCharacterMovement()->bWantsToCrouch = false;
	}

	const bool bCanEnterSprint = CanEnterSprint();
	if (m_bWantsToSprint && !m_bIsSprinting && bCanEnterSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = m_SprintSpeed;
		m_bIsSprinting = true;
		m_InteractionUserComponent->SetInteractionUserType(InteractionUserType::PlayerSprint);
	}
	if (m_bIsSprinting && (!m_bWantsToSprint || !bCanEnterSprint))
	{
		GetCharacterMovement()->MaxWalkSpeed = m_DefaultSpeed;
		m_bIsSprinting = false;
		m_InteractionUserComponent->SetInteractionUserType(InteractionUserType::Player);
	}
}

void AFirstPersonPlayerCharacter::LockCamera(float maxYaw, float maxPitch)
{
	m_bIsInputLocked = true;
	
	m_currentMaxPitchOffset = maxPitch;
	m_currentMaxYawOffset = maxYaw;
}


void AFirstPersonPlayerCharacter::UnlockCamera()
{
	m_bIsInputLocked = false;

	m_currentMaxPitchOffset = m_MaxHeadPitch;
	m_currentMaxYawOffset = m_MaxHeadYaw;
}

bool AFirstPersonPlayerCharacter::CanEnterSprint() const
{
	if (!GetCharacterMovement()->IsMovingOnGround() || m_bForcedToCrouch)
		return false;
	
	FVector movementVector = GetCharacterMovement()->GetPawnOwner()->GetVelocity();
	
	if (movementVector.SizeSquared() < 0.1f)
		return false;
	
	movementVector.Normalize();
	
	const FTransform currentTransform = GetCharacterMovement()->GetPawnOwner()->GetActorTransform();
	
	const FVector forwardVector = currentTransform.GetUnitAxis(EAxis::X);

	const float angle = FMath::RadiansToDegrees(acos(FVector::DotProduct(movementVector, forwardVector)));
	
	return angle < 60;
}


