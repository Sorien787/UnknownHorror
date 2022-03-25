// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FirstPersonPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFirstPersonPlayerCharacter::AFirstPersonPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pCameraBoomArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Cam Boom Arm"));
	m_pCameraBoomArm->SetupAttachment(GetMesh(), "cameraSocket");
	
	m_CharacterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(m_CharacterCamera != nullptr);
	m_CharacterCamera->SetupAttachment(GetMesh(), USpringArmComponent::SocketName);
	m_CharacterCamera->bUsePawnControlRotation = true;
	
	m_InteractionComponent = CreateDefaultSubobject<UInteractionUserComponent>(TEXT("Interaction User"));
	check(m_InteractionComponent != nullptr);
	
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
	GetCharacterMovement()->MaxWalkSpeed = m_DefaultSpeed;
	m_InteractionComponent->AddInteractionEnterBox(m_EnterBoxComponent);
	m_InteractionComponent->AddInteractionExitBox(m_ExitBoxComponent);
}

// Called every frame
void AFirstPersonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SprintStateUpdate();
}

// Called to bind functionality to input
void AFirstPersonPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);	

	PlayerInputComponent->BindAxis("MoveForward", this, &AFirstPersonPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFirstPersonPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookRight", this, &AFirstPersonPlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFirstPersonPlayerCharacter::AddControllerPitchInput);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFirstPersonPlayerCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFirstPersonPlayerCharacter::StopJump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFirstPersonPlayerCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFirstPersonPlayerCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFirstPersonPlayerCharacter::SprintEnd);
	
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFirstPersonPlayerCharacter::StartInteractions);
}


void AFirstPersonPlayerCharacter::MoveForward(float Value)
{
	FVector Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AFirstPersonPlayerCharacter::MoveRight(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AFirstPersonPlayerCharacter::StartJump()
{
	bPressedJump = true;
}

void AFirstPersonPlayerCharacter::ToggleCrouch()
{
	UCharacterMovementComponent* pCharacterMovement = GetCharacterMovement();
	const bool isCrouch = pCharacterMovement->bWantsToCrouch;
	GetCharacterMovement()->bWantsToCrouch = !isCrouch;
}

void AFirstPersonPlayerCharacter::SprintStateUpdate()
{
	if (m_bWantsToSprint)
	{
		GetCharacterMovement()->bWantsToCrouch = false;
	}

	const bool bCanEnterSprint = CanEnterSprint();
	if (m_bWantsToSprint && !m_bIsSprinting && bCanEnterSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = m_SprintSpeed;
		m_bIsSprinting = true;
	}
	if (m_bIsSprinting && (!m_bWantsToSprint || !bCanEnterSprint))
	{
		GetCharacterMovement()->MaxWalkSpeed = m_DefaultSpeed;
		m_bIsSprinting = false;
	}
}

void AFirstPersonPlayerCharacter::SprintStart()
{
	m_bWantsToSprint = true;
}

void AFirstPersonPlayerCharacter::SprintEnd()
{
	m_bWantsToSprint = false;
}

void AFirstPersonPlayerCharacter::StopJump()
{
	bPressedJump = false;
}

bool AFirstPersonPlayerCharacter::CanEnterSprint() const
{


	if (!GetCharacterMovement()->IsMovingOnGround())
		return false;
	
	FVector movementVector = GetCharacterMovement()->GetPawnOwner()->GetVelocity();
	
	if (movementVector.SizeSquared() < 0.1f)
		return false;
	
	movementVector.Normalize();
	
	const FTransform currentTransform = GetCharacterMovement()->GetPawnOwner()->GetActorTransform();
	
	const FVector forwardVector = currentTransform.GetUnitAxis(EAxis::X);

	const float angle = FMath::RadiansToDegrees(acos(FVector::DotProduct(movementVector, forwardVector)));
	FString val = FString::SanitizeFloat(angle);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, *val);
	return angle < 60;
}

void AFirstPersonPlayerCharacter::StartInteractions()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("A"));
	m_InteractionComponent->OnInteractButtonPressed();
}



