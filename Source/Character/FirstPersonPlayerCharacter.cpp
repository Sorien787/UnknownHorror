// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FirstPersonPlayerCharacter.h"

#include "DrawDebugHelpers.h"
#include "Interaction/InteractionUserComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Common/UnrealUtilities.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

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

void AFirstPersonPlayerCharacter::ClampViewOffsetToMaximum(float maximumPitch, float maximumYaw)
{
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


	m_HeartBeatSFXTrigger = UnrealUtilities::RisingEdgeTrigger<float>(0.0f);
	m_pCameraBoomArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Cam Boom Arm"));
	m_pCameraBoomArm->SetupAttachment(GetMesh(), "cameraSocket");
	
	m_CharacterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(m_CharacterCamera != nullptr);
	m_CharacterCamera->SetupAttachment(m_pCameraBoomArm, USpringArmComponent::SocketName);
	m_CharacterCamera->bUsePawnControlRotation = false;
	
	m_InteractionPlayerComponent = CreateDefaultSubobject<UInteractionPlayerComponent>(TEXT("Interaction Player Component"));
	check(m_InteractionPlayerComponent != nullptr);
	
	m_EnterInteractionSphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Interaction Enter Sphere Collider"));
	check(m_EnterInteractionSphereCollider != nullptr);
	m_EnterInteractionSphereCollider->SetupAttachment(GetRootComponent());

	m_ExitInteractionSphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Interaction Exit Sphere Collider"));
	check(m_ExitInteractionSphereCollider != nullptr);
	m_ExitInteractionSphereCollider->SetupAttachment(GetRootComponent());

	m_pHazeEffectComponent = CreateDefaultSubobject<UHazeEffectComponent>(TEXT("Haze Effect"));
	check(m_pHazeEffectComponent != nullptr);
	
	m_DroneAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Drone Audio"));
	check(m_DroneAudioComponent != nullptr);

	m_RingingAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Ring Audio"));
	check(m_RingingAudioComponent != nullptr);

	m_HeartbeatAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Heart Thump Audio"));
	check(m_HeartbeatAudioComponent != nullptr);
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

	m_InteractionPlayerComponent->AddInteractionEnterShape(m_EnterInteractionSphereCollider);
	m_InteractionPlayerComponent->AddInteractionExitShape(m_ExitInteractionSphereCollider);

	m_DroneAudioComponent->SetVolumeMultiplier(0.0f);
	m_RingingAudioComponent->SetVolumeMultiplier(0.0f);

	m_eCurrentMovementState = EMovementState::Walking;
	m_eForcedMovementState = EMovementState::None;
	m_eDesiredMovementState = EMovementState::Walking;
	OnEnterMovementState(m_eCurrentMovementState);
}

// Called every frame
void AFirstPersonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MovementStateUpdate(DeltaTime);
	LookStateUpdate(DeltaTime);
	CrouchStateUpdate(DeltaTime);
	InteractionStateUpdate(DeltaTime);
	HazeDispersalUpdate(DeltaTime);
	CameraEffectsUpdate(DeltaTime);
	AudioEffectsUpdate(DeltaTime);
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

bool AFirstPersonPlayerCharacter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	ECollisionChannel DefaultSightCollisionChannel = ECollisionChannel::ECC_Visibility;
	UWorld* pWorld = GetWorld();
	const FName TraceTag("VisiblityTraceTag");
	pWorld->DebugDrawTraceTag = TraceTag;
	FCollisionQueryParams params = FCollisionQueryParams(SCENE_QUERY_STAT(AILineOfSight), true, IgnoreActor);
	params.TraceTag = TraceTag;
	FHitResult HitResult;
    const bool bHit = pWorld->LineTraceSingleByChannel(HitResult, ObserverLocation, GetActorLocation()
    	, DefaultSightCollisionChannel
    	, params);
	if (bHit && HitResult.GetActor() == this)
	{
		OutSeenLocation = HitResult.Location;
		return true;
	}
	return false;
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
	m_eDesiredMovementState = EMovementState::Crouching;
	EvaluateNewMovementState();
}

void AFirstPersonPlayerCharacter::EndCrouch()
{
	m_eDesiredMovementState = EMovementState::Walking;
	EvaluateNewMovementState();
}

void AFirstPersonPlayerCharacter::ToggleCrouch()
{
	if (m_bIsInputLocked)
		return;
	
	if (m_eDesiredMovementState == EMovementState::Crouching)
		m_eDesiredMovementState = EMovementState::Walking;
	else
		m_eDesiredMovementState = EMovementState::Crouching;
	EvaluateNewMovementState();
}

void AFirstPersonPlayerCharacter::ToggleProne()
{
	if (m_bIsInputLocked)
		return;

	if (m_eDesiredMovementState == EMovementState::Prone)
		m_eDesiredMovementState = EMovementState::Walking;
	else
		m_eDesiredMovementState = EMovementState::Prone;
	EvaluateNewMovementState();
}


void AFirstPersonPlayerCharacter::StartSprint()
{
	m_eDesiredMovementState = EMovementState::Sprinting;
	EvaluateNewMovementState();
}

void AFirstPersonPlayerCharacter::StopSprint()
{
	m_eDesiredMovementState = EMovementState::Walking;
	EvaluateNewMovementState();
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
	if (!m_InteractionPlayerComponent)
		return;
	m_InteractionPlayerComponent->OnInteractButtonPressed();
}

void AFirstPersonPlayerCharacter::InteractionStateUpdate(float DeltaTime)
{
	if (!m_InteractionPlayerComponent)
		return;
	
	const bool bShouldLockCamera = m_InteractionPlayerComponent->IsInteracting();
	if (bShouldLockCamera && !m_bIsInputLocked)
	{
		LockCamera(m_InteractionPlayerComponent->GetAnimCameraYaw(), m_InteractionPlayerComponent->GetAnimCameraPitch());		
	}
	if (!bShouldLockCamera && m_bIsInputLocked)
	{
		UnlockCamera();
	}
}

void AFirstPersonPlayerCharacter::HazeDispersalUpdate(float DeltaTime)
{
	m_fCurrentHaze +=  DeltaTime * FMath::Min(m_MaximumHazeApplication, m_pHazeEffectComponent->GetCurrentHazeStrength());
	m_fCurrentHaze -= DeltaTime * m_HazeDispersalByCurrentHaze.EditorCurveData.Eval(m_fCurrentHaze / m_MaximumGainedHaze);
	m_fCurrentHaze = FMath::Clamp(m_fCurrentHaze, 0.0f, m_MaximumGainedHaze);
}

void AFirstPersonPlayerCharacter::CameraEffectsUpdate(float DeltaTime)
{

	const float effectiveHazeStrength = m_fCurrentHaze / m_MaximumGainedHaze;
	
	m_fCurrentHeartbeatTime += DeltaTime / m_HazeEffectPulseTimeByHazeStrength.EditorCurveData.Eval(effectiveHazeStrength);
	m_fCurrentHeartbeatTime = fmod(m_fCurrentHeartbeatTime, 1.0f);
	const float heartBeatPoint = m_HazeEffectPulseProfile.EditorCurveData.Eval(m_fCurrentHeartbeatTime);

	if(m_HeartBeatSFXTrigger.PollEdge(heartBeatPoint))
	{
		m_HeartbeatAudioComponent->Play();
	}
	const float heartbeatVol = m_HazeEffectPulseVolumeByHazeStrength.EditorCurveData.Eval(effectiveHazeStrength);
	m_HeartbeatAudioComponent->SetVolumeMultiplier(heartbeatVol);
	const float heartbeatPitch = m_HazeEffectPulsePitchByHazeStrength.EditorCurveData.Eval(effectiveHazeStrength);
	m_HeartbeatAudioComponent->SetPitchMultiplier(heartbeatPitch);
	const float ringingVol = m_HazeEffectRingingVolumeByHazeStrength.EditorCurveData.Eval(effectiveHazeStrength);
	m_RingingAudioComponent->SetVolumeMultiplier(ringingVol);
	const float droneVol = m_HazeEffectDroneVolumeByHazeStrength.EditorCurveData.Eval(effectiveHazeStrength);
	m_DroneAudioComponent->SetVolumeMultiplier(droneVol);
	
	UMaterialParameterCollectionInstance* pMaterialParameterCollectionInstance = GetWorld()->GetParameterCollectionInstance(m_VisualsMaterialParameterCollection);
	pMaterialParameterCollectionInstance->SetScalarParameterValue("PulseHeight", heartBeatPoint);
	pMaterialParameterCollectionInstance->SetScalarParameterValue("Strength", effectiveHazeStrength);

	m_CharacterCamera->PostProcessSettings.BloomIntensity = m_HazeEffectBloomByHazeStrength.EditorCurveData.Eval(effectiveHazeStrength);
	m_CharacterCamera->PostProcessSettings.SceneFringeIntensity = m_HazeEffectChromaticAbberation.EditorCurveData.Eval(effectiveHazeStrength);
}

void AFirstPersonPlayerCharacter::AudioEffectsUpdate(float DeltaTime)
{
	const float effectiveHazeStrength = m_fCurrentHaze / m_MaximumGainedHaze;
	const float desiredFrequency = m_AudioLowPassByHazeStrength.EditorCurveData.Eval(effectiveHazeStrength);
	m_InWorldSoundClass->Properties.LowPassFilterFrequency = desiredFrequency;
}

void AFirstPersonPlayerCharacter::LookStateUpdate(float DeltaTime)
{
	const bool tryEnterLookState = m_bIsInputLocked || m_bWantsToLook;

	// use the controller for !m_bIsLooking

	// use the head for m_bIsLooking, + the additional angles

	if (tryEnterLookState && !m_bIsLooking)
	{
		m_bIsLooking = true;

		// we'll be looking towards the control rotation
		// we want to look relative to our head bone
		// so figure out head bone to control rotation
		// and set our current pitch offset and yaw offsets to match
		const FQuat controlQuat = FQuat(GetControlRotation());
	
		const FTransform cameraBoneTransform = GetMesh()->GetSocketTransform(m_HeadBoneName);
		const FQuat cameraBoneQuat = cameraBoneTransform.GetRotation();

		const FQuat controlQuat_to_cameraBoneQuat = controlQuat.Inverse() * cameraBoneQuat;

		const float pitch = -controlQuat_to_cameraBoneQuat.Euler().Y;
		const float yaw = controlQuat_to_cameraBoneQuat.Euler().Z;

		m_currentPitchOffset = pitch;
		m_currentYawOffset = yaw;

		bUseControllerRotationYaw = false;
	}
	
	if (!tryEnterLookState && m_bIsLooking)
	{
		m_bIsLooking = false;
		// we'll be looking along head bone + offsets
		// but we want to be looking along our control rotation
		// so set our control reotation to match our look state as best as possible
		
		const FQuat currentHeadQuat = FQuat(m_CharacterCamera->GetComponentRotation());
		const float pitch = currentHeadQuat.Euler().Y;
		const float yaw = currentHeadQuat.Euler().Z;

		// however.
		// this control quat could be wrapped all the way over 90 degrees, or under 90.
		// so we want to limit the pitch to plus/minus 90.

		const float pitchAfterLimit = FMath::Clamp(pitch, -89, 89);
		
		const FQuat controlQuat = FQuat::MakeFromEuler(FVector(0.0f, pitchAfterLimit, yaw));
		FRotator actorRotation = GetActorRotation();
		const FQuat actorRot = FQuat::MakeFromRotator(actorRotation);
		const float pitch2 = actorRot.Euler().Y;
		const float quat2 = actorRot.Euler().Z;
		if (pitch2>quat2)
		{int i = 0; i++;}
		Controller->SetControlRotation(FRotator(controlQuat));
		
		bUseControllerRotationYaw = true;
	}


	FQuat targetHeadQuat;

	if (m_bIsLooking)
	{
		const FTransform headBoneTransform = GetMesh()->GetSocketTransform(m_HeadBoneName);
		const FQuat headBoneRotation = headBoneTransform.GetRotation();
		ClampViewOffsetToShoulder(DeltaTime);
		const FQuat extraQuat = FQuat::MakeFromEuler(FVector(0.0f, m_currentPitchOffset, m_currentYawOffset));

		targetHeadQuat = headBoneRotation * extraQuat;
		GetController()->SetControlRotation(FRotator(targetHeadQuat));
	}
	else
	{
		const FQuat rotation = FQuat(GetControlRotation());
		
		ClampViewOffsetToZero(DeltaTime);
	
		const FQuat extraQuat = FQuat::MakeFromEuler(FVector(m_currentPitchOffset, m_currentYawOffset, 0.0f));
		
		targetHeadQuat =  rotation * extraQuat;

	}

	const FQuat currentHeadQuat = FQuat(m_CharacterCamera->GetComponentRotation());

	FQuat actualHeadQuat = UnrealUtilities::QuatSpringInterpCD(currentHeadQuat, targetHeadQuat, m_currentHeadVelocity, DeltaTime, m_CameraLookNormalizedSpeed, 100000.0f);

	m_CharacterCamera->SetWorldRotation(actualHeadQuat);
}

void AFirstPersonPlayerCharacter::CrouchStateUpdate(float DeltaTime)
{
	// if we're not crouching, check if we need to crouch to move underneath something that we're near
	if (m_eForcedMovementState != EMovementState::None && m_eForcedMovementState != EMovementState::Crouching)
		return;
	// init params
	const UCapsuleComponent* MyCapsuleComponent = GetCapsuleComponent();
	constexpr float SweepInflation =15.0f;
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

	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, this);
	CapsuleParams.bDebugQuery = true;
	FCollisionResponseParams ResponseParam;
	GetMesh()->InitSweepCollisionParams(CapsuleParams, ResponseParam);
	//DrawDebugCapsule(MyWorld, PawnLocation, CapsuleExtent.Z, CapsuleExtent.X, FQuat::Identity, FColor::Emerald);
	// make the overlap test
	const bool needToCrouch = MyWorld->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, ECC_WorldStatic, CapsuleCollision, CapsuleParams, ResponseParam);

	m_eForcedMovementState = needToCrouch ? EMovementState::Crouching : EMovementState::None;
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

void AFirstPersonPlayerCharacter::OnEnterMovementState(EMovementState newState)
{
	switch (newState)
	{
	case(EMovementState::Sprinting):
		GetCharacterMovement()->MaxWalkSpeed = m_SprintSpeed;
		m_InteractionPlayerComponent->SetInteractionUserType(InteractionUserType::PlayerSprint);
		break;
	case(EMovementState::Prone):
		GetCharacterMovement()->MaxWalkSpeed = m_ProneSpeed;
		break;
	case (EMovementState::Walking):
		GetCharacterMovement()->MaxWalkSpeed = m_DefaultSpeed;
		break;
	case(EMovementState::Crouching):
		GetCharacterMovement()->MaxWalkSpeed = m_DefaultCrouchSpeed;
		GetCharacterMovement()->bWantsToCrouch = true;
		break;
	default:
		break;
	}
}

void AFirstPersonPlayerCharacter::OnExitMovementState(EMovementState oldState)
{
	switch (oldState)
	{
	case(EMovementState::Sprinting):
		m_InteractionPlayerComponent->SetInteractionUserType(InteractionUserType::Player);
		break;
	case(EMovementState::Crouching):
		GetCharacterMovement()->bWantsToCrouch = false;
		break;
	default:
		break;
	}
}

void AFirstPersonPlayerCharacter::EvaluateNewMovementState()
{
	const EMovementState movementStateToSet = GetMovementStateToSet();

	if (movementStateToSet == m_eCurrentMovementState)
		return;

	OnExitMovementState(m_eCurrentMovementState);
	m_eCurrentMovementState = movementStateToSet;
	OnEnterMovementState(m_eCurrentMovementState);
}

AFirstPersonPlayerCharacter::EMovementState AFirstPersonPlayerCharacter::GetMovementStateToSet()
{
	if (m_eForcedMovementState != EMovementState::None)
		return m_eForcedMovementState;

	switch (m_eDesiredMovementState)
	{
	case (EMovementState::Sprinting):
		return CanEnterSprint() ? EMovementState::Sprinting : EMovementState::Walking;
	default:
		return m_eDesiredMovementState;
	}
}

bool AFirstPersonPlayerCharacter::CanEnterSprint() const
{
	if (!GetCharacterMovement()->IsMovingOnGround() || m_eForcedMovementState != EMovementState::None)
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

void AFirstPersonPlayerCharacter::MovementStateUpdate(float DeltaTime)
{
	CrouchStateUpdate(DeltaTime);
	EvaluateNewMovementState();
}


